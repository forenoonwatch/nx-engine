#pragma once

#include <engine/resource/resource-loader.hpp>

#include <engine/rendering/vertex-array.hpp>

class VertexArrayLoader final : public ResourceLoader<VertexArrayLoader, VertexArray> {
	public:
		Memory::SharedPointer<VertexArray> load(const IndexedModel& model, uint32 usage = GL_STATIC_DRAW) const {
			auto& context = RenderContext::ref();

			return Memory::make_shared<VertexArray>(context, model, usage);
		}
	private:
};
