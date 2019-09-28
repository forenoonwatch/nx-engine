#pragma once

#include <engine/core/common.hpp>

#include <engine/rendering/render-context.hpp>
#include <engine/rendering/vertex-array.hpp>
#include <engine/rendering/texture.hpp>

class Game;

class GameRenderContext : public RenderContext {
	public:
		GameRenderContext(Game& game);

		void renderMesh(VertexArray& vertexArray, Texture& texture);

		void flush();
	private:
		NULL_COPY_AND_ASSIGN(GameRenderContext);
};
