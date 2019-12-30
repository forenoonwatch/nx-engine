#pragma once

#include <engine/core/tree-map.hpp>
#include <engine/core/array-list.hpp>

#include <engine/rendering/vertex-array.hpp>
#include <engine/rendering/shader.hpp>
#include <engine/rendering/render-target.hpp>
#include <engine/rendering/material.hpp>

#include <engine/game/camera.hpp>

#include <engine/math/math.hpp>

class GaussianBlur;

class GameRenderContext : public RenderContext {
	public:
		GameRenderContext(uint32 width = 1200, uint32 height = 800,
				float fieldOfView = Math::toRadians(70.f), float zNear = 0.1f,
				float zFar = 100.f);

		void resize(uint32 width, uint32 height);

		inline void updateCameraBuffer();

		inline void renderMesh(VertexArray& vertexArray, Material& material,
				const Matrix4f& transform);
		inline void renderSkybox(CubeMap& skybox, Sampler& sampler);

		inline void setDiffuseIBL(CubeMap& diffuseIBL) {
			this->diffuseIBL = &diffuseIBL;
		}

		inline void setSpecularIBL(CubeMap& specularIBL) {
			this->specularIBL = &specularIBL;
		}

		inline void setBrdfLUT(Texture& brdfLUT) { this->brdfLUT = &brdfLUT; }

		inline RenderTarget& getTarget() { return target; }
		inline RenderTarget& getScreen() { return screen; }

		inline CubeMap& getDiffuseIBL() { return *diffuseIBL; }
		inline CubeMap& getSpecularIBL() { return *specularIBL; }

		inline Sampler& getNearestSampler() { return nearestSampler; }
		inline Sampler& getLinearSampler() { return linearSampler; }
		inline Sampler& getLinearMipmapSampler() {
			return linearMipmapSampler;
		}

		inline VertexArray& getSkyboxCube() { return *skyboxCube; }

		inline Camera& getCamera() { return camera; }

		virtual ~GameRenderContext();
		
		/* Pipeline Systems */
		void clear();

		void applyLighting();

		void flushStaticMeshes();

		void flush();
	private:
		NULL_COPY_AND_ASSIGN(GameRenderContext);

		Texture colorBuffer;
		Texture normalBuffer;
		Texture lightingBuffer;
		Texture brightBuffer;
		Texture depthBuffer;

		RenderTarget target;
		RenderTarget screen;

		Shader staticMeshShader;
		Shader skyboxShader;
		Shader lightingShader;
		Shader blurShader;
		Shader bloomShader;
		Shader toneMapShader;
		Shader screenRenderShader;

		UniformBuffer sceneDataBuffer;
		UniformBuffer lightDataBuffer;

		Sampler nearestSampler;
		Sampler linearSampler;
		Sampler linearMipmapSampler;

		VertexArray* skyboxCube;

		GaussianBlur* bloomBlur;

		CubeMap* diffuseIBL;
		CubeMap* specularIBL;

		Texture* brdfLUT;

		float fieldOfView;
		float zNear;
		float zFar;

		Camera camera;

		TreeMap<Pair<VertexArray*, Material*>, ArrayList<Matrix4f>> staticMeshes;
};

inline void GameRenderContext::updateCameraBuffer() {
	sceneDataBuffer.update(&camera.view[3], sizeof(Vector3f));
	sceneDataBuffer.update(&camera.viewProjection, sizeof(Vector3f)
			+ sizeof(Vector2f) + 3 * sizeof(float), sizeof(Matrix4f));
	sceneDataBuffer.update(&camera.iViewProjection, sizeof(Vector3f)
			+ sizeof(Vector2f) + 3 * sizeof(float)
			+ sizeof(Matrix4f), sizeof(Matrix4f));
}

inline void GameRenderContext::renderMesh(VertexArray& vertexArray,
		Material& material, const Matrix4f& transform) {
	staticMeshes[std::make_pair(&vertexArray, &material)].push_back(transform);
}

inline void GameRenderContext::renderSkybox(CubeMap& skybox,
		Sampler& sampler) {
	skyboxShader.setSampler("skybox", skybox, sampler, 0);
	draw(target, skyboxShader, *skyboxCube, GL_TRIANGLES);
}

