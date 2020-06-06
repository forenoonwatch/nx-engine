#pragma once

#include <engine/resource/resource-loader.hpp>

#include <engine/animation/rig.hpp>

class RigLoader final : public ResourceLoader<RigLoader, Rig> {
	public:
		Memory::SharedPointer<Rig> load(Rig& rig) const {
			return Memory::make_shared<Rig>(rig);
		}
	private:
};
