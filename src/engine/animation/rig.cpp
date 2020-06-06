#include "engine/animation/rig.hpp"

void Rig::resetBones() {
	for (uint32 i = 0; i < bones.size(); ++i) {
		transformSet[i] = globalInverseTransform * bones[i].localTransform;
	}
}

void Rig::calcRootBone() {
	for (uint32 i = 0; i < bones.size(); ++i) {
		if (bones[i].parent < 0) {
			DEBUG_LOG_TEMP("Root joint is joint %d", i);
			rootBone = i;

			return;
		}
	}
}
