#pragma once

#include <engine/core/service.hpp>

#include <engine/core/tree-map.hpp>
#include <engine/core/array-list.hpp>

#include <engine/rendering/shader.hpp>
#include <engine/rendering/render-target.hpp>
#include <engine/rendering/camera.hpp>

#include <engine/math/math.hpp>

class GaussianBlur;
class VertexArray;
class Material;
class Rig;

class RenderSystem final : public Service<RenderSystem> {
	public:
		RenderSystem(RenderContext& context, uint32 width, uint32 height,
				float fieldOfView, float zNear, float zFar);

		void resize(uint32 width, uint32 height);

		void updateCamera();

		void drawStaticMesh(VertexArray& vertexArray, Material& material,
				const Matrix4f& transform);
		void drawRiggedMesh(VertexArray& vertexArray, Material& material,
				Rig& rig, const Matrix4f& transform);

		void renderSkybox(CubeMap& skybox, Sampler& sampler);
		void renderSkybox();

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

		~RenderSystem();
		
		void clear();
		void applyLighting();
		void flushStaticMeshes();
		void flushRiggedMeshes();
		void flush();
	private:
		NULL_COPY_AND_ASSIGN(RenderSystem);

		RenderContext* context;

		Texture colorBuffer;
		Texture normalBuffer;
		Texture lightingBuffer;
		Texture brightBuffer;
		Texture depthBuffer;

		RenderTarget target;
		RenderTarget screen;

		Shader staticMeshShader;
		Shader riggedMeshShader;

		Shader skyboxShader;
		Shader lightingShader;
		Shader blurShader;
		Shader bloomShader;
		Shader toneMapShader;
		Shader screenRenderShader;

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
		TreeMap<Pair<VertexArray*, Material*>, ArrayList<Pair<Rig*, Matrix4f>>> riggedMeshes;
};
