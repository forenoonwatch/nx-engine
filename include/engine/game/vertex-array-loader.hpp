#pragma once

#include <engine/game/resource-loader.hpp>

#include <engine/rendering/vertex-array.hpp>

#include <engine/core/asset-loader.hpp>
#include <engine/core/application.hpp>

class VertexArrayLoader final
		: public ResourceLoader<VertexArrayLoader, VertexArray> {
	public:
		Memory::SharedPointer<VertexArray> load(const char* fileName,
				const IndexedModel::AllocationHints& hints) const {
			ArrayList<IndexedModel> models;

			if (!AssetLoader::loadAssets(fileName, hints, models)) {
				DEBUG_LOG(LOG_ERROR, "Static Mesh",
						"Failed to load static mesh data from: %s", fileName);
				return nullptr;
			}

			return Memory::make_shared<VertexArray>(
					*static_cast<RenderContext*>(&Application::getInstance()),
					models[0], GL_STATIC_DRAW);
		}
	private:
};

