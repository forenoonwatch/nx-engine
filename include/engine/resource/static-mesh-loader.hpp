#pragma once

#include <engine/resource/resource-loader.hpp>
#include <engine/resource/asset-loader.hpp>

#include <engine/rendering/vertex-array.hpp>

class StaticMeshLoader final : public ResourceLoader<StaticMeshLoader, VertexArray> {
	public:
		Memory::SharedPointer<VertexArray> load(const StringView& fileName) const {
			auto& context = RenderContext::ref();

			ArrayList<IndexedModel> models;

			if (!AssetLoader::loadStaticMeshes(fileName, models)) {
				DEBUG_LOG(LOG_ERROR, "Static Mesh", "Failed to load static mesh data from: %s", fileName.data());
				return nullptr;
			}

			return Memory::make_shared<VertexArray>(context, models[0], GL_STATIC_DRAW);
		}
	private:
};
