#include "engine/ecs/ecs-system.hpp"

bool ECSSystem::isValid() const {
	for (uint32 i = 0; i < componentFlags.size(); ++i) {
		if ((componentFlags[i] & ECSSystem::FLAG_OPTIONAL) == 0) {
			return true;
		}
	}

	return false;
}

bool ECSSystemList::removeSystem(ECSSystem& system) {
	for (uint32 i = 0; i < systems.size(); ++i) {
		if (&system == systems[i]) {
			systems.erase(std::begin(systems) + i);
			return true;
		}
	}

	return false;
}

