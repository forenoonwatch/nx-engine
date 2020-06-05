#pragma once

#include <engine/resource/resource-loader.hpp>
#include <engine/resource/asset-loader.hpp>

#include <engine/rendering/indexed-model.hpp>

class ModelLoader final : public ResourceLoader<ModelLoader, IndexedModel> {
	public:
		Memory::SharedPointer<IndexedModel> load(const StringView& fileName) const {
			ArrayList<IndexedModel> models;

			if (!AssetLoader::loadStaticMeshes(fileName, models)) {
				DEBUG_LOG(LOG_ERROR, "Static Mesh", "Failed to load static mesh data from: %s", fileName.data());
				return nullptr;
			}

			return Memory::make_shared<IndexedModel>(models[0]);
		}
	private:
};

