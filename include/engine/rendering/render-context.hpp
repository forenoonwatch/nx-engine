#pragma once

#include <engine/core/common.hpp>
#include <engine/core/string.hpp>
#include <engine/core/service.hpp>
#include <engine/core/array-list.hpp>
#include <engine/core/hash-map.hpp>

#include <engine/rendering/draw-params.hpp>

#include <GL/glew.h>

class Shader;
class UniformBuffer;

class VertexArray;
class TransformFeedback;
class InputStreamBuffer;

class RenderTarget;

class RenderQuery;

class RenderContext final : public Service<RenderContext> {
	public:
		RenderContext();

		void awaitFinish();

		void draw(RenderTarget& target, Shader& shader, VertexArray& vertexArray,
				const DrawParams& drawParams, uint32 primitive, uint32 numInstances = 1);

		void drawArray(RenderTarget& target, Shader& shader, VertexArray& vertexArray,
				const DrawParams& drawParams,
				uint32 bufferIndex, uint32 primitive, uint32 numInstances = 1,
				uint32 numElements = 0);
		void drawArray(Shader& shader, VertexArray& vertexArray, const DrawParams& drawParams,
				uint32 bufferIndex, uint32 primitive, uint32 numInstances = 1,
				uint32 numElements = 0);
		void drawArray(Shader& shader, InputStreamBuffer& isb, const DrawParams& drawParams,
				uint32 numElements, uint32 primitive);

		void drawTransformFeedback(RenderTarget& target, Shader& shader,
				TransformFeedback& transformFeedback, const DrawParams& drawParams,
				uint32 primitive);
		void drawTransformFeedback(Shader& shader, TransformFeedback& transformFeedback,
				const DrawParams& drawParams, uint32 primitive);
		
		void compute(Shader& shader, uint32 numGroupsX,
				uint32 numGroupsY = 1, uint32 numGroupsZ = 1);

		void beginTransformFeedback(Shader& shader, TransformFeedback& tfb,
				const DrawParams& drawParams, uint32 primitive);
		void endTransformFeedback();

		void beginQuery(RenderQuery& query);
		void endQuery(RenderQuery& query);

		uint32 getVersion();
		String getShaderVersion();

		void setViewport(uint32, uint32);

		void setShader(uint32);
		void setVertexArray(uint32);
		void setTransformFeedback(uint32);

		void setRenderTarget(uint32 fbo, uint32 bufferType = GL_FRAMEBUFFER);

		void setDrawParams(const DrawParams& params);

		void setFaceCullMode(enum DrawParams::FaceCullMode mode);

		void setDrawBuffers(uint32 numBuffers);

		void setWriteDepth(bool writeDepth);
		void setDepthFunc(enum DrawParams::DrawFunc depthFunc);

		void setRasterizerDiscard(bool discard);
		void setBlending(enum DrawParams::BlendFunc srcBlend,
				enum DrawParams::BlendFunc destBlend);

		void setScissorTest(bool enable, uint32 startX, uint32 startY,
				uint32 width, uint32 height);

		void setStencilTest(bool enabled);
		void setStencilFunc(enum DrawParams::DrawFunc stencilFunc,
				uint32 stencilTestMask, int32 stencilComparisonVal);
		void setStencilOp(enum DrawParams::StencilOp stencilFail,
				enum DrawParams::StencilOp stencilPass,
				enum DrawParams::StencilOp stencilPassDepthFail);
		void setStencilWriteMask(uint32 mask);

		Memory::SharedPointer<UniformBuffer> addUniformBuffer(const String& name,
				uintptr dataSize, uint32 usage);

		Memory::WeakPointer<UniformBuffer> getUniformBuffer(const String& name);

		~RenderContext();

		static uint32 calcInternalFormat(uint32 pixelFormat, bool compressed);
		static uint32 calcBaseFormat(uint32 pixelFormat);
	private:
		NULL_COPY_AND_ASSIGN(RenderContext);

		uint32 version;
		String shaderVersion;

		uint32 viewportWidth;
		uint32 viewportHeight;

		DrawParams drawState;

		uint32 currentShader;
		uint32 currentVertexArray;
		uint32 currentTFB;

		uint32 currentRenderSource;
		uint32 currentRenderTarget;

		HashMap<String, Memory::WeakPointer<UniformBuffer>> uniformBuffers;
		ArrayList<bool> uniformBufferBindings;

		static uint32 attachments[4];

		friend class UniformBuffer;
		friend class RenderTarget;

		uint32 findFreeUBOBinding();
};
