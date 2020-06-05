#pragma once

#include <engine/core/string-view.hpp>

#include <engine/rendering/indexed-model.hpp>

namespace AssetLoader {
	bool loadStaticMeshes(const StringView& fileName, ArrayList<IndexedModel>& models);
};
