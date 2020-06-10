#include "engine/rendering/render-system.hpp"

#include <engine/animation/rig.hpp>

#include <engine/rendering/vertex-array.hpp>
#include <engine/rendering/material.hpp>
#include <engine/rendering/gaussian-blur.hpp>
#include <engine/rendering/font.hpp>

static void initSkyboxCube(IndexedModel&);
static void initScreenQuad(IndexedModel&);
static void initUIQuad(IndexedModel&);

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
		, riggedMeshShader(context)

		, skyboxShader(context)
		, lightingShader(context)
		, blurShader(context)
		, bloomShader(context)
		, toneMapShader(context)
		, screenRenderShader(context)

		, textureQuadShader(context)

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
	drawParams.faceCullMode = DrawParams::FACE_CULL_BACK;
	drawParams.depthFunc = DrawParams::DRAW_FUNC_LEQUAL;
	drawParams.writeDepth = true;

	target.addTextureTarget(normalBuffer, GL_COLOR_ATTACHMENT0, 1);
	target.addTextureTarget(lightingBuffer, GL_COLOR_ATTACHMENT0, 2);

	target.addTextureTarget(brightBuffer, GL_COLOR_ATTACHMENT0, 3);

	target.addTextureTarget(depthBuffer, GL_DEPTH_ATTACHMENT);

	staticMeshShader.load("./res/shaders/static-mesh-deferred.glsl");
	riggedMeshShader.load("./res/shaders/rigged-mesh-deferred.glsl");

	skyboxShader.load("./res/shaders/skybox-deferred.glsl");
	lightingShader.load("./res/shaders/deferred-lighting.glsl");
	blurShader.load("./res/shaders/gaussian-blur-shader.glsl");
	bloomShader.load("./res/shaders/bloom-shader.glsl");
	toneMapShader.load("./res/shaders/tone-map-shader.glsl");
	screenRenderShader.load("./res/shaders/screen-render-shader.glsl");

	textureQuadShader.load("./res/shaders/textured-quad-shader.glsl");

	IndexedModel cube;
	initSkyboxCube(cube);

	IndexedModel screenQuadModel;
	initScreenQuad(screenQuadModel);

	IndexedModel uiQuadModel;
	initUIQuad(uiQuadModel);

	skyboxCube = new VertexArray(context, cube,
			GL_STATIC_DRAW);

	screenQuad = new VertexArray(context, screenQuadModel, DrawParams::USAGE_STATIC_READ);
	uiQuad = new VertexArray(context, uiQuadModel, DrawParams::USAGE_STATIC_READ);

	bloomBlur = new GaussianBlur(context, blurShader,
			brightBuffer);

	camera.projection = Math::perspective(fieldOfView, (float)width
			/ (float)height, zNear, zFar);
	camera.iProjection = Math::inverse(camera.projection);

	auto lightDataBuffer = context.getUniformBuffer("LightingData").lock();

	Vector3f lightDir = Math::normalize(Vector3f(1, -1, 1));

	lightDataBuffer->set("sunlightDir", lightDir);

	lightDataBuffer->set("ambientLight", 0.f);
	lightDataBuffer->set("specularStrength", 15.f);
	lightDataBuffer->set("specularBlend", 128.f);

	Vector3f fogColor(202.f / 255.f, 243.f / 255.f, 246.f / 255.f);

	lightDataBuffer->set("fogColor", fogColor);
	lightDataBuffer->set("fogDensity", 0.001f);
	lightDataBuffer->set("fogGradient", 2.f);

	auto sceneDataBuffer = context.getUniformBuffer("SceneData").lock();

	Vector2f sceneDims(width, height);

	sceneDataBuffer->set("displaySize", sceneDims);
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

	auto sceneDataBuffer = context->getUniformBuffer("SceneData").lock();

	Vector2f sceneDims(width, height);

	sceneDataBuffer->set("displaySize", sceneDims);
}

void RenderSystem::updateCamera() {
	camera.update();

	auto sceneDataBuffer = context->getUniformBuffer("SceneData").lock();

	sceneDataBuffer->set({"cameraPosition", "viewProjection", "invVP"},
			Vector3f(camera.view[3]), camera.viewProjection, camera.iViewProjection);
}

void RenderSystem::drawStaticMesh(VertexArray& vertexArray,
		Material& material, const Matrix4f& transform) {
	staticMeshes[std::make_pair(&vertexArray, &material)].push_back(transform);
}

void RenderSystem::drawRiggedMesh(VertexArray& vertexArray,
		Material& material, Rig& rig, const Matrix4f& transform) {
	riggedMeshes[std::make_pair(&vertexArray, &material)].push_back(std::make_pair(&rig, transform));
}

void RenderSystem::drawTextureQuad(Texture& texture, const Vector4f& positions,
		const Vector4f& scales, const Vector3f& color) {
	auto& qd = textureQuads[&texture];

	qd.positionPairs.push_back(positions);
	qd.scalePairs.push_back(scales);
	qd.colors.push_back(color);
}

void RenderSystem::drawText(Font& font, const StringView& text,
		float x, float y, const Vector3f& color) {
	for (char c : text) {
		auto& cd = font.getCharacter(c);

		float xPos = x + cd.bearingX;
		float yPos = y - cd.sizeY + cd.bearingY;

		x += cd.advance;

		drawTextureQuad(font.getTexture(), Vector4f(xPos, yPos, cd.texCoordData[0], cd.texCoordData[1]),
				Vector4f(static_cast<float>(cd.sizeX), static_cast<float>(cd.sizeY), cd.texCoordData[2], cd.texCoordData[3]),
				color);
	}
}

void RenderSystem::renderSkybox(CubeMap& skybox,
		Sampler& sampler) {
	const Matrix4f mvp = Math::translate(camera.viewProjection, camera.getPosition());

	skyboxCube->updateBuffer(1, &mvp, sizeof(Matrix4f));

	skyboxShader.setSampler("skybox", skybox, sampler, 0);
	context->draw(target, skyboxShader, *skyboxCube, drawParams, GL_TRIANGLES);
}

void RenderSystem::renderSkybox() {
	renderSkybox(getSpecularIBL(), getLinearMipmapSampler());
}

RenderSystem::~RenderSystem() {
	delete skyboxCube;
	delete screenQuad;
	delete uiQuad;
	delete bloomBlur;
}

void RenderSystem::clear() {
	screen.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	target.setDrawBuffers(4);
	target.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderSystem::applyLighting() {
	drawParams.writeDepth = false;

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

	drawScreenQuad(target, lightingShader);
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

			staticMeshShader.setFloat("heightScale", material->displacementScale);
		}

		vertexArray->updateBuffer(5, &pair.second[0],
				sizeof(Matrix4f) * numTransforms);

		context->draw(target, staticMeshShader, *vertexArray,
				drawParams, GL_TRIANGLES, numTransforms);
	}

	staticMeshes.clear();
}

void RenderSystem::flushRiggedMeshes() {
	Material* currentMaterial = nullptr;
	Material* material;

	VertexArray* vertexArray;

	uintptr numTransforms;

	auto animBuf = context->getUniformBuffer("AnimationData").lock();

	for (auto& pair : riggedMeshes) {
		numTransforms = pair.second.size();

		if (numTransforms == 0) {
			continue;
		}

		vertexArray = pair.first.first;
		material = pair.first.second;

		if (material != currentMaterial) {
			currentMaterial = material;

			riggedMeshShader.setSampler("diffuse", *material->diffuse,
					linearMipmapSampler, 0);
			riggedMeshShader.setSampler("normalMap", *material->normalMap,
					linearMipmapSampler, 1);
			riggedMeshShader.setSampler("materialMap",
					*material->materialMap, linearMipmapSampler, 2);
			riggedMeshShader.setSampler("depthMap", *material->displacementMap,
					linearMipmapSampler, 3);

			riggedMeshShader.setFloat("heightScale", material->displacementScale);
		}

		for (auto& rigTF : pair.second) {
			animBuf->update(rigTF.first->getTransformSet(), Rig::MAX_JOINTS * sizeof(Matrix4f));

			vertexArray->updateBuffer(7, &rigTF.second, sizeof(Matrix4f));
			context->draw(target, riggedMeshShader, *vertexArray,
					drawParams, GL_TRIANGLES);
		}
	}

	riggedMeshes.clear();
}

void RenderSystem::flush() {
	//bloomBlur->update();

	target.setDrawBuffers(1);

	/* Merge bloom with main scene */ 
	bloomShader.setSampler("scene", colorBuffer,
			nearestSampler, 0);
	bloomShader.setSampler("brightBlur", brightBuffer,
			nearestSampler, 1);
	drawScreenQuad(target, bloomShader);

	/* Tone map colors */
	toneMapShader.setSampler("screen", colorBuffer,
			nearestSampler, 0);
	drawScreenQuad(target, toneMapShader);

	/* Render to screen */
	screenRenderShader.setSampler("screen", colorBuffer,
			nearestSampler, 0);
	drawScreenQuad(screen, screenRenderShader);

	drawParams.writeDepth = true;
	// TODO: Take note that this causes rendering issues when not present
	// assumingly because its setting the write depth while the screen framebuffer is set
}

void RenderSystem::flushTexturedQuads() {
	size_t numInstances;

	drawParams.sourceBlend = DrawParams::BLEND_FUNC_SRC_ALPHA;
	drawParams.destBlend = DrawParams::BLEND_FUNC_ONE_MINUS_SRC_ALPHA;

	for (auto& pair : textureQuads) {
		numInstances = pair.second.positionPairs.size();

		if (numInstances == 0) {
			continue;
		}

		textureQuadShader.setSampler("diffuse", *pair.first, linearSampler, 0);

		uiQuad->updateBuffer(1, pair.second.positionPairs.data(), numInstances * sizeof(Vector4f));
		uiQuad->updateBuffer(2, pair.second.scalePairs.data(), numInstances * sizeof(Vector4f));
		uiQuad->updateBuffer(3, pair.second.colors.data(), numInstances * sizeof(Vector3f));

		drawUIQuad(screen, textureQuadShader, numInstances);

		pair.second.positionPairs.clear();
		pair.second.scalePairs.clear();
	}

	drawParams.sourceBlend = DrawParams::BLEND_FUNC_NONE;
	drawParams.destBlend = DrawParams::BLEND_FUNC_NONE;
}

void RenderSystem::drawScreenQuad(RenderTarget& target, Shader& shader, uint32 numInstances) {
	context->draw(target, shader, *screenQuad, drawParams, GL_TRIANGLES, numInstances);
}

void RenderSystem::drawUIQuad(RenderTarget& target, Shader& shader, uint32 numInstances) {
	context->draw(target, shader, *uiQuad, drawParams, GL_TRIANGLES, numInstances);
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

static void initScreenQuad(IndexedModel& screenQuadModel) {
	screenQuadModel.allocateElement(2); // vertex

	screenQuadModel.addElement2f(0, -1.f, -1.f);
	screenQuadModel.addElement2f(0, -1.f,  1.f);
	screenQuadModel.addElement2f(0,  1.f, -1.f);
	screenQuadModel.addElement2f(0,  1.f,  1.f);

	screenQuadModel.addIndices3i(2, 1, 0);
	screenQuadModel.addIndices3i(1, 2, 3);
}

static void initUIQuad(IndexedModel& uiQuadModel) {
	uiQuadModel.allocateElement(4); // vertex
	uiQuadModel.allocateElement(4); // quad position
	uiQuadModel.allocateElement(4); // quad scale
	uiQuadModel.allocateElement(3); // color

	uiQuadModel.setInstancedElementStartIndex(1);

	uiQuadModel.addElement4f(0, 0.f, 0.f, 0.f, 1.f);
	uiQuadModel.addElement4f(0, 0.f, 1.f, 0.f, 0.f);
	uiQuadModel.addElement4f(0, 1.f, 0.f, 1.f, 1.f);
	uiQuadModel.addElement4f(0, 1.f, 1.f, 1.f, 0.f);

	uiQuadModel.addIndices3i(2, 1, 0);
	uiQuadModel.addIndices3i(1, 2, 3);
}
