#pragma once

#include <engine/rendering/texture.hpp>
#include <engine/rendering/render-target.hpp>

class Shader;
class VertexArray;

class GaussianBlur {
	public:
		GaussianBlur(RenderContext& context, Shader& blurShader,
				Texture& blurTarget);

		void update(VertexArray& screenQuad, DrawParams& drawParams);

		void resize(uint32 width, uint32 height);

		inline Texture& getBuffer0() { return blurTarget; }
		inline Texture& getBuffer1() { return buffer; }
	private:
		RenderContext* context;

		Shader& blurShader;
		
		Texture& blurTarget;
		Texture buffer;

		RenderTarget target0;
		RenderTarget target1;

		bool horizontal;
};

