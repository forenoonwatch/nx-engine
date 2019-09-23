#pragma once

#include "engine/core/string.hpp"

#include "engine/rendering/indexed-model.hpp"

namespace AssetLoader {
	bool loadAssets(const String& fileName,
			ArrayList<IndexedModel>& models);
};
