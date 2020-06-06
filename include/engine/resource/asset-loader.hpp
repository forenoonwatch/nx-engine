#pragma once

#include <engine/core/string-view.hpp>

#include <engine/animation/rig.hpp>
#include <engine/animation/animation.hpp>

#include <engine/rendering/indexed-model.hpp>

namespace AssetLoader {
	bool loadAssets(const StringView& fileName, ArrayList<IndexedModel>& models,
			ArrayList<Rig>& rigs, ArrayList<Animation>& animations);
};
