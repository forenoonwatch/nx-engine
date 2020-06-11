#include "engine/rendering/gaussian-blur.hpp"

#include <engine/rendering/shader.hpp>
#include <engine/rendering/vertex-array.hpp>

#define NUM_PASSES 10
#define LOCAL_BLOCK_SIZE 16

GaussianBlur::GaussianBlur(RenderContext& context, Shader& blurShader,
			Texture& blurTarget)
		: context(&context)
		, blurShader(blurShader)
		, blurTarget(blurTarget)
		, buffer(context, blurTarget.getWidth(),
				blurTarget.getHeight(), GL_RGBA32F)
		, target0(context, blurTarget, blurTarget.getWidth(),
				blurTarget.getHeight(), GL_COLOR_ATTACHMENT0)
		, target1(context, buffer, blurTarget.getWidth(),
				blurTarget.getHeight(), GL_COLOR_ATTACHMENT0) {}

void GaussianBlur::update(VertexArray& screenQuad, DrawParams& drawParams) {
	horizontal = true;

	for (uint32 i = 0; i < NUM_PASSES; ++i) {
		blurShader.setInt("horizontal", horizontal);
		blurShader.setSampler("image", horizontal ? blurTarget : buffer, 0);

		context->draw(horizontal ? target1 : target0, blurShader,
				screenQuad, drawParams, GL_TRIANGLES);

		horizontal = !horizontal;
	}
}

void GaussianBlur::resize(uint32 width, uint32 height) {
	target0.resize(width, height);
	target1.resize(width, height);

	buffer.resize(width, height);
}

