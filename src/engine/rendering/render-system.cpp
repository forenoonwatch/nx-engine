#include "engine/rendering/render-system.hpp"

#include <engine/rendering/gaussian-blur.hpp>

static void initSkyboxCube(IndexedModel&);

RenderSystem::RenderSystem(RenderContext& context, uint32 width, uint32 height,
			float fieldOfView, float zNear, float zFar)
		: context(&context)

		, colorBuffer(context, width, height, GL_RGBA32F)
		, normalBuffer(context, width, height, GL_RGBA32F)
		, lightingBuffer(context, width, height, GL_RGBA32F)
		, brightBuffer(context, width, height, GL_RGBA32F)
		, depthBuffer(context, width, height,
				GL_DEPTH_COMPONENT, nullptr, GL_DEPTH_COMPONENT, GL_FLOAT)
	
		, target(context, colorBuffer, GL_COLOR_ATTACHMENT0)
		, screen(context, width, height)

		, staticMeshShader(context)
		, skyboxShader(context)
		, lightingShader(context)
		, blurShader(context)
		, bloomShader(context)
		, toneMapShader(context)
		, screenRenderShader(context)

		, sceneDataBuffer(context, sizeof(Vector3f)
				+ sizeof(Vector2f) + 2 * sizeof(Matrix4f), GL_STREAM_DRAW, 0)
		, lightDataBuffer(context, 1 * sizeof(Vector3f)
				+ 3 * sizeof(float) + sizeof(Vector3f)
				+ 2 * sizeof(float), GL_DYNAMIC_DRAW, 1)
		
		, nearestSampler(context, GL_NEAREST, GL_NEAREST)
		, linearSampler(context, GL_LINEAR, GL_LINEAR,
				GL_REPEAT, GL_REPEAT)
		, linearMipmapSampler(context,
				GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR)
		
		, diffuseIBL(nullptr)
		, specularIBL(nullptr)
		, brdfLUT(nullptr)

		, fieldOfView(fieldOfView)
		, zNear(zNear)
		, zFar(zFar)
		, camera({Matrix4f(1.f), Matrix4f(1.f), Matrix4f(1.f),
				Matrix4f(1.f), Matrix4f(1.f)}) {
	target.addTextureTarget(normalBuffer, GL_COLOR_ATTACHMENT0, 1);
	target.addTextureTarget(lightingBuffer, GL_COLOR_ATTACHMENT0, 2);

	target.addTextureTarget(brightBuffer, GL_COLOR_ATTACHMENT0, 3);

	target.addTextureTarget(depthBuffer, GL_DEPTH_ATTACHMENT);

	staticMeshShader.load("./res/shaders/static-mesh-deferred.glsl");
	skyboxShader.load("./res/shaders/skybox-deferred.glsl");
	lightingShader.load("./res/shaders/deferred-lighting.glsl");
	blurShader.load("./res/shaders/gaussian-blur-shader.glsl");
	bloomShader.load("./res/shaders/bloom-shader.glsl");
	toneMapShader.load("./res/shaders/tone-map-shader.glsl");
	screenRenderShader.load("./res/shaders/screen-render-shader.glsl");

	IndexedModel cube;
	initSkyboxCube(cube);

	skyboxCube = new VertexArray(context, cube,
			GL_STATIC_DRAW);

	bloomBlur = new GaussianBlur(context, blurShader,
			brightBuffer);

	camera.projection = Math::perspective(fieldOfView, (float)width
			/ (float)height, zNear, zFar);
	camera.iProjection = Math::inverse(camera.projection);

	float lightData[] = {0.f, 15.f, 128.f};
	Vector3f lightDir = Math::normalize(Vector3f(1, -1, 1));
	lightDataBuffer.update(&lightDir, sizeof(Vector3f));
	lightDataBuffer.update(lightData, sizeof(Vector3f), sizeof(lightData));

	float fogData[] = {202.f / 255.f, 243.f / 255.f, 246.f / 255.f,
		0.001f, 2.f};

	lightDataBuffer.update(fogData, sizeof(Vector3f) + sizeof(lightData)
			+ 2 * sizeof(float), sizeof(fogData));

	Vector2f sceneDims(width, height);
	sceneDataBuffer.update(&sceneDims, sizeof(Vector3f)
			+ sizeof(float), sizeof(Vector2f));
}

void RenderSystem::resize(uint32 width, uint32 height) {
	target.resize(width, height);
	screen.resize(width, height);

	colorBuffer.resize(width, height);
	normalBuffer.resize(width, height);
	lightingBuffer.resize(width, height);
	brightBuffer.resize(width, height);
	depthBuffer.resize(width, height);

	bloomBlur->resize(width, height);

	camera.projection = Math::perspective(fieldOfView,
			(float)width / (float)height, zNear, zFar);
}

void RenderSystem::updateCamera() {
	camera.update();

	sceneDataBuffer.update(&camera.view[3], sizeof(Vector3f));
	sceneDataBuffer.update(&camera.viewProjection, sizeof(Vector3f)
			+ sizeof(Vector2f) + 3 * sizeof(float), sizeof(Matrix4f));
	sceneDataBuffer.update(&camera.iViewProjection, sizeof(Vector3f)
			+ sizeof(Vector2f) + 3 * sizeof(float)
			+ sizeof(Matrix4f), sizeof(Matrix4f));
}

void RenderSystem::drawStaticMesh(VertexArray& vertexArray,
		Material& material, const Matrix4f& transform) {
	staticMeshes[std::make_pair(&vertexArray, &material)].push_back(transform);
}

void RenderSystem::renderSkybox(CubeMap& skybox,
		Sampler& sampler) {
	const Matrix4f mvp = Math::translate(camera.viewProjection, camera.getPosition());

	skyboxCube->updateBuffer(1, &mvp, sizeof(Matrix4f));

	skyboxShader.setSampler("skybox", skybox, sampler, 0);
	context->draw(target, skyboxShader, *skyboxCube, GL_TRIANGLES);
}

void RenderSystem::renderSkybox() {
	renderSkybox(getSpecularIBL(), getLinearMipmapSampler());
}

RenderSystem::~RenderSystem() {
	delete skyboxCube;
	delete bloomBlur;
}

void RenderSystem::clear() {
	screen.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	target.setDrawBuffers(4);
	target.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderSystem::applyLighting() {
	context->setWriteDepth(false);

	// apply lighting
	lightingShader.setSampler("colorBuffer", colorBuffer,
			nearestSampler, 0);
	lightingShader.setSampler("normalBuffer", normalBuffer,
			nearestSampler, 1);
	lightingShader.setSampler("lightingBuffer", lightingBuffer,
			nearestSampler, 2);
	
	lightingShader.setSampler("depthBuffer", depthBuffer,
			nearestSampler, 3);
	
	lightingShader.setSampler("irradianceMap", *diffuseIBL,
			linearMipmapSampler, 4);
	lightingShader.setSampler("prefilterMap", *specularIBL,
			linearSampler, 5);
	lightingShader.setSampler("brdfLUT", *brdfLUT,
			nearestSampler, 6);

	context->drawQuad(target, lightingShader);
}

void RenderSystem::flushStaticMeshes() {
	Material* currentMaterial = nullptr;
	Material* material;

	VertexArray* vertexArray;
	uintptr numTransforms;

	for (auto& pair : staticMeshes) {
		numTransforms = pair.second.size();

		if (numTransforms == 0) {
			continue;
		}

		vertexArray = pair.first.first;
		material = pair.first.second;

		if (material != currentMaterial) {
			currentMaterial = material;

			staticMeshShader.setSampler("diffuse", *material->diffuse,
					linearMipmapSampler, 0);
			staticMeshShader.setSampler("normalMap", *material->normalMap,
					linearMipmapSampler, 1);
			staticMeshShader.setSampler("materialMap",
					*material->materialMap, linearMipmapSampler, 2);
			staticMeshShader.setSampler("depthMap", *material->displacementMap,
					linearMipmapSampler, 3);
		}

		vertexArray->updateBuffer(4, &pair.second[0],
				sizeof(Matrix4f) * numTransforms);

		context->draw(target, staticMeshShader, *vertexArray,
				GL_TRIANGLES, numTransforms);
	}

	staticMeshes.clear();
}

void RenderSystem::flush() {
	bloomBlur->update();

	target.setDrawBuffers(1);

	/* Merge bloom with main scene */ 
	bloomShader.setSampler("scene", colorBuffer,
			nearestSampler, 0);
	bloomShader.setSampler("brightBlur", brightBuffer,
			nearestSampler, 1);
	context->drawQuad(target, bloomShader);

	/* Tone map colors */
	toneMapShader.setSampler("screen", colorBuffer,
			nearestSampler, 0);
	context->drawQuad(target, toneMapShader);

	/* Render to screen */
	screenRenderShader.setSampler("screen", colorBuffer,
			nearestSampler, 0);
	context->drawQuad(screen, screenRenderShader);

	context->setWriteDepth(true);
}

static void initSkyboxCube(IndexedModel& model) {
	model.allocateElement(3); // position
	model.allocateElement(16); // transform
	
	model.setInstancedElementStartIndex(1);

	for (float z = -1.f; z <= 1.f; z += 2.f) {
		for (float y = -1.f; y <= 1.f; y += 2.f) {
			for (float x = -1.f; x <= 1.f; x += 2.f) {
				model.addElement3f(0, x, y, z);
			}
		}
	}

	// back
	model.addIndices3i(0, 1, 2);
	model.addIndices3i(3, 2, 1);

	// front
	model.addIndices3i(6, 5, 4);
	model.addIndices3i(5, 6, 7);

	// bottom
	model.addIndices3i(4, 1, 0);
	model.addIndices3i(1, 4, 5);

	// top
	model.addIndices3i(2, 3, 6);
	model.addIndices3i(7, 6, 3);

	// left
	model.addIndices3i(0, 2, 6);
	model.addIndices3i(6, 4, 0);

	// right
	model.addIndices3i(1, 5, 7);
	model.addIndices3i(7, 3, 1);
}
