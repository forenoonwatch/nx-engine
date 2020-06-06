#pragma once

#include <engine/resource/resource-loader.hpp>

#include <engine/animation/animation.hpp>

class AnimationLoader final : public ResourceLoader<AnimationLoader, Animation> {
	public:
		Memory::SharedPointer<Animation> load(Animation& animation) const {
			return Memory::make_shared<Animation>(animation);
		}
	private:
};
