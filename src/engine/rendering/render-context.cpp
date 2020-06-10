#include "engine/rendering/render-context.hpp"

#include "engine/rendering/shader.hpp"

#include "engine/rendering/vertex-array.hpp"
#include "engine/rendering/transform-feedback.hpp"
#include "engine/rendering/input-stream-buffer.hpp"

#include "engine/rendering/render-target.hpp"

#include "engine/rendering/render-query.hpp"

#include "engine/rendering/indexed-model.hpp"

inline static void initScreenQuad(IndexedModel&);

static void GLAPIENTRY errorCallback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

uint32 RenderContext::attachments[] = {GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};

RenderContext::RenderContext()
		: version(0)
		, shaderVersion("")
		, viewportWidth(0)
		, viewportHeight(0)
		, currentShader(0)
		, currentVertexArray(0)
		, currentTFB(0)
		, currentRenderSource(0)
		, currentRenderTarget(0) {
	glEnable(GL_TEXTURE_2D);

	//glEnable(GL_DEBUG_OUTPUT);
	//glDebugMessageCallback(errorCallback, 0);

	IndexedModel screenQuadModel;
	initScreenQuad(screenQuadModel);

	screenQuad = new VertexArray(*this, screenQuadModel, GL_STATIC_DRAW);
}

void RenderContext::awaitFinish() {
	glFinish();
}

void RenderContext::draw(RenderTarget& target, Shader& shader,
		VertexArray& vertexArray, const DrawParams& drawParams, uint32 primitive,
		uint32 numInstances) {
	setRenderTarget(target.getID());
	setViewport(target.getWidth(), target.getHeight());

	setShader(shader.getID());
	setVertexArray(vertexArray.getID());

	setDrawParams(drawParams);

	switch (numInstances) {
		case 0:
			return;
		case 1:
			glDrawElements(primitive, (GLsizei)vertexArray.getNumElements(), GL_UNSIGNED_INT, 0);
			return;
		default:
			glDrawElementsInstanced(primitive, (GLsizei)vertexArray.getNumElements(),
					GL_UNSIGNED_INT, 0, numInstances);
	}
}

void RenderContext::drawArray(RenderTarget& target, Shader& shader,
		VertexArray& vertexArray, const DrawParams& drawParams,
		uint32 bufferIndex, uint32 primitive, uint32 numInstances, uint32 numElements) {
	setRenderTarget(target.getID());
	setViewport(target.getWidth(), target.getHeight());

	setShader(shader.getID());
	setVertexArray(vertexArray.getID());

	setDrawParams(drawParams);

	glBindBuffer(GL_ARRAY_BUFFER, vertexArray.getBuffer(bufferIndex));

	if (numElements == 0) {
		numElements = vertexArray.getNumElements();
	}

	switch (numInstances) {
		case 0:
			return;
		case 1:
			glDrawArrays(primitive, 0, numElements);
			return;
		default:
			glDrawArraysInstanced(primitive, 0, numElements, numInstances);
	}
}

void RenderContext::drawArray(Shader& shader, VertexArray& vertexArray,
		const DrawParams& drawParams,
		uint32 bufferIndex, uint32 primitive, uint32 numInstances,
		uint32 numElements) {
	setShader(shader.getID());
	setVertexArray(vertexArray.getID());

	setDrawParams(drawParams);

	glBindBuffer(GL_ARRAY_BUFFER, vertexArray.getBuffer(bufferIndex));

	if (numElements == 0) {
		numElements = vertexArray.getNumElements();
	}

	switch (numInstances) {
		case 0:
			return;
		case 1:
			glDrawArrays(primitive, 0, numElements);
			return;
		default:
			glDrawArraysInstanced(primitive, 0, numElements, numInstances);
	}
}

void RenderContext::drawArray(Shader& shader, InputStreamBuffer& isb,
		const DrawParams& drawParams, uint32 numElements, uint32 primitive) {
	setShader(shader.getID());
	setVertexArray(isb.getReadArray());

	setDrawParams(drawParams);

	glBindBuffer(GL_ARRAY_BUFFER, isb.getReadBuffer());

	glDrawArrays(primitive, 0, numElements);
}

void RenderContext::drawTransformFeedback(RenderTarget& target, Shader& shader,
		TransformFeedback& transformFeedback, const DrawParams& drawParams, uint32 primitive) {
	setRenderTarget(target.getID());
	setViewport(target.getWidth(), target.getHeight());

	setShader(shader.getID());
	setVertexArray(transformFeedback.getReadArray());

	setDrawParams(drawParams);

	glDrawTransformFeedback(primitive, transformFeedback.getReadFeedback());
}

void RenderContext::drawTransformFeedback(Shader& shader,
		TransformFeedback& transformFeedback,
		const DrawParams& drawParams, uint32 primitive) {
	setShader(shader.getID());
	setVertexArray(transformFeedback.getReadArray());

	setDrawParams(drawParams);

	glDrawTransformFeedback(primitive, transformFeedback.getReadFeedback());
}

void RenderContext::drawQuad(RenderTarget& target, Shader& shader,
		const DrawParams& drawParams) {
	setRenderTarget(target.getID());
	setViewport(target.getWidth(), target.getHeight());

	setShader(shader.getID());
	setVertexArray(screenQuad->getID());

	setDrawParams(drawParams);

	glDrawElements(GL_TRIANGLES, (GLsizei)screenQuad->getNumElements(),
			GL_UNSIGNED_INT, 0);
}

void RenderContext::compute(Shader& shader, uint32 numGroupsX,
		uint32 numGroupsY, uint32 numGroupsZ) {
	setShader(shader.getID());
	glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
}

void RenderContext::beginTransformFeedback(Shader& shader, TransformFeedback& tfb,
		const DrawParams& drawParams, uint32 primitive) {
	setShader(shader.getID());
	setTransformFeedback(tfb.getWriteFeedback());

	setDrawParams(drawParams);

	glBeginTransformFeedback(primitive);
}

void RenderContext::endTransformFeedback() {
	glEndTransformFeedback();
}

void RenderContext::beginQuery(RenderQuery& query) {
	glBeginQuery(query.getType(), query.getID());
}

void RenderContext::endQuery(RenderQuery& query) {
	glEndQuery(query.getType());
}

uint32 RenderContext::getVersion() {
	if (version != 0) {
		return version;
	}

	int32 major, minor;

	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	version = (uint32)(major * 100 + minor * 10);

	return version;
}

String RenderContext::getShaderVersion() {
	if (!shaderVersion.empty()) {
		return shaderVersion;
	}

	uint32 ver = getVersion();

	if (ver >= 330) {
		shaderVersion = std::to_string(ver);
	}
	else if (ver >= 320) {
		shaderVersion = "330"; // really should be 150 but bugs on my laptop
	}
	else if (ver >= 310) {
		shaderVersion = "140";
	}
	else if (ver >= 300) {
		shaderVersion = "130";
	}
	else if (ver >= 210) {
		shaderVersion = "120";
	}
	else {
		shaderVersion = "110";
	}

	return shaderVersion;
}

void RenderContext::setViewport(uint32 width, uint32 height) {
	if (width != viewportWidth || height != viewportHeight) {
		viewportWidth = width;
		viewportHeight = height;

		glViewport(0, 0, width, height);
	}
}

void RenderContext::setShader(uint32 shader) {
	if (currentShader != shader) {
		currentShader = shader;
		glUseProgram(shader);
	}
}

void RenderContext::setVertexArray(uint32 vao) {
	if (currentVertexArray != vao) {
		currentVertexArray = vao;
		glBindVertexArray(vao);
	}
}

void RenderContext::setTransformFeedback(uint32 tfb) {
	if (currentTFB != tfb) {
		currentTFB = tfb;
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfb);
	}
}

void RenderContext::setRenderTarget(uint32 fbo, uint32 bufferType) {
	switch (bufferType) {
		case GL_FRAMEBUFFER:
			if (currentRenderSource != fbo || currentRenderTarget != fbo) {
				currentRenderSource = fbo;
				currentRenderTarget = fbo;

				glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			}

			return;
		case GL_READ_FRAMEBUFFER:
			if (currentRenderSource != fbo) {
				currentRenderSource = fbo;

				glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
			}

			return;
		case GL_DRAW_FRAMEBUFFER:
			if (currentRenderTarget != fbo) {
				currentRenderTarget = fbo;

				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
			}

			return;
	}
}

Memory::SharedPointer<UniformBuffer> RenderContext::addUniformBuffer(const String& name,
		uintptr dataSize, uint32 usage) {
	auto ubo = Memory::make_shared<UniformBuffer>(*this, dataSize, usage, findFreeUBOBinding());

	uniformBuffers[name] = Memory::WeakPointer<UniformBuffer>(ubo);

	return ubo;
}

Memory::WeakPointer<UniformBuffer> RenderContext::getUniformBuffer(const String& name) {
	return uniformBuffers[name];
}

RenderContext::~RenderContext() {
	delete screenQuad;
}

void RenderContext::setDrawParams(const DrawParams& params) {
	setFaceCullMode(params.faceCullMode);
	
	setDrawBuffers(params.numDrawBuffers);

	setWriteDepth(params.writeDepth);
	setDepthFunc(params.depthFunc);

	setRasterizerDiscard(params.discardRasterizer);
	setBlending(params.sourceBlend, params.destBlend);

	setScissorTest(params.scissorTest, params.scissorStartX, params.scissorStartY,
			params.scissorWidth, params.scissorHeight);

	setStencilTest(params.stencilTest);
	setStencilFunc(params.stencilFunc, params.stencilTestMask,
			params.stencilComparisonVal);
	setStencilOp(params.stencilFail, params.stencilPass,
			params.stencilPassDepthFail);
	setStencilWriteMask(params.stencilWriteMask);
}

void RenderContext::setFaceCullMode(enum DrawParams::FaceCullMode mode) {
	if (mode == drawState.faceCullMode) {
		return;
	}

	if (mode == DrawParams::FACE_CULL_NONE) {
		glDisable(GL_CULL_FACE);
	}
	else if (drawState.faceCullMode == DrawParams::FACE_CULL_NONE) {
		glEnable(GL_CULL_FACE);
		glCullFace(mode);
	}
	else {
		glCullFace(mode);
	}

	drawState.faceCullMode = mode;
}

void RenderContext::setDrawBuffers(uint32 numBuffers) {
	if (numBuffers == drawState.numDrawBuffers) {
		return;
	}

	glDrawBuffers(numBuffers, attachments);

	drawState.numDrawBuffers = numBuffers;
}

void RenderContext::setWriteDepth(bool writeDepth) {
	if (writeDepth == drawState.writeDepth) {
		return;
	}

	glDepthMask(writeDepth);

	drawState.writeDepth = writeDepth;
}

void RenderContext::setDepthFunc(enum DrawParams::DrawFunc depthFunc) {
	if (depthFunc == drawState.depthFunc) {
		return;
	}

	glDepthFunc(depthFunc);

	drawState.depthFunc = depthFunc;
}

void RenderContext::setRasterizerDiscard(bool discard) {
	if (discard == drawState.discardRasterizer) {
		return;
	}

	if (discard) {
		glEnable(GL_RASTERIZER_DISCARD);
	}
	else {
		glDisable(GL_RASTERIZER_DISCARD);
	}

	drawState.discardRasterizer = discard;
}

void RenderContext::setBlending(enum DrawParams::BlendFunc srcBlend,
		enum DrawParams::BlendFunc destBlend) {
	if (srcBlend == drawState.sourceBlend && destBlend == drawState.destBlend) {
		return;
	}

	if (srcBlend == DrawParams::BLEND_FUNC_NONE
			|| destBlend == DrawParams::BLEND_FUNC_NONE) {
		glDisable(GL_BLEND);
	}
	else if (drawState.sourceBlend == DrawParams::BLEND_FUNC_NONE 
			|| drawState.destBlend == DrawParams::BLEND_FUNC_NONE) {
		glEnable(GL_BLEND);
		glBlendFunc(srcBlend, destBlend);
	}
	else {
		glBlendFunc(srcBlend, destBlend);
	}

	drawState.sourceBlend = srcBlend;
	drawState.destBlend = destBlend;
}

void RenderContext::setScissorTest(bool enable, uint32 startX, uint32 startY,
		uint32 width, uint32 height) {
	if (!enable) {
		if (drawState.scissorTest) {
			glDisable(GL_SCISSOR_TEST);
			drawState.scissorTest = false;
		}

		return;
	}

	if (!drawState.scissorTest) {
		glEnable(GL_SCISSOR_TEST);
		drawState.scissorTest = true;
	}

	glScissor(startX, startY, width, height);
}

void RenderContext::setStencilTest(bool enabled) {
	if (enabled == drawState.stencilTest) {
		return;
	}

	if (enabled) {
		glEnable(GL_STENCIL_TEST);
	}
	else {
		glDisable(GL_STENCIL_TEST);
	}

	drawState.stencilTest = enabled;
}

void RenderContext::setStencilFunc(enum DrawParams::DrawFunc stencilFunc,
		uint32 stencilTestMask, int32 stencilComparisonVal) {
	if (stencilFunc == drawState.stencilFunc
			&& stencilTestMask == drawState.stencilTestMask
			&& stencilComparisonVal == drawState.stencilComparisonVal) {
		return;
	}

	glStencilFunc(stencilFunc, stencilTestMask, stencilComparisonVal);

	drawState.stencilFunc = stencilFunc;
	drawState.stencilTestMask = stencilTestMask;
	drawState.stencilComparisonVal = stencilComparisonVal;
}

void RenderContext::setStencilOp(enum DrawParams::StencilOp stencilFail,
		enum DrawParams::StencilOp stencilPass,
		enum DrawParams::StencilOp stencilPassDepthFail) {
	if (stencilFail == drawState.stencilFail
			&& stencilPass == drawState.stencilPass
			&& stencilPassDepthFail == drawState.stencilPassDepthFail) {
		return;
	}

	glStencilOp(stencilFail, stencilPassDepthFail, stencilPass);

	drawState.stencilFail = stencilFail;
	drawState.stencilPass = stencilPass;
	drawState.stencilPassDepthFail = stencilPassDepthFail;
}

void RenderContext::setStencilWriteMask(uint32 mask) {
	if (mask == drawState.stencilWriteMask) {
		return;
	}

	glStencilMask(mask);

	drawState.stencilWriteMask = mask;
}

uint32 RenderContext::findFreeUBOBinding() {
	for (uint32 i = 0; i < uniformBufferBindings.size(); ++i) {
		if (!uniformBufferBindings[i]) {
			uniformBufferBindings[i] = true;
			return i;
		}
	}

	uniformBufferBindings.push_back(true);

	return uniformBufferBindings.size() - 1;
}

uint32 RenderContext::calcInternalFormat(uint32 pixelFormat, bool compressed) {
	switch (pixelFormat) {
		case GL_RGB:
			return compressed ? GL_COMPRESSED_SRGB_S3TC_DXT1_EXT
					: GL_RGB;
		case GL_RGBA:
			return compressed ? GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT
					: GL_RGBA;
		case GL_RED:
		case GL_RG:
		case GL_RGBA32F:
		case GL_DEPTH_COMPONENT:
		case GL_DEPTH_STENCIL:
			return pixelFormat;
		default:
			DEBUG_LOG(LOG_ERROR, "Texture",
					"%d is not a valid pixel format", pixelFormat);
			return 0;
	}
}

uint32 RenderContext::calcBaseFormat(uint32 pixelFormat) {
	switch (pixelFormat) {
		case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
			return GL_RGB;
		case GL_RGBA32F:
		case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
			return GL_RGBA;
		case GL_RED:
		case GL_RG:
		case GL_RGB:
		case GL_RGBA:
		case GL_DEPTH_COMPONENT:
		case GL_DEPTH_STENCIL:
			return pixelFormat;
		default:
			DEBUG_LOG(LOG_ERROR, "Texture",
					"%d is not a valid pixel format", pixelFormat);
			return 0;
	}
}

inline static void initScreenQuad(IndexedModel& screenQuadModel) {
	screenQuadModel.allocateElement(2); // position

	screenQuadModel.addElement2f(0, -1.f, -1.f);
	screenQuadModel.addElement2f(0, -1.f,  1.f);
	screenQuadModel.addElement2f(0,  1.f, -1.f);
	screenQuadModel.addElement2f(0,  1.f,  1.f);

	screenQuadModel.addIndices3i(2, 1, 0);
	screenQuadModel.addIndices3i(1, 2, 3);
}

static void GLAPIENTRY errorCallback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
		return;
	}

	DEBUG_LOG_TEMP2(message);
}
