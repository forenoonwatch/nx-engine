#pragma once

#include "engine/rendering/indexed-model.hpp"

namespace AssetLoader {
	bool loadAssets(const std::string& fileName,
			std::vector<IndexedModel>& models);
};
