#pragma once

#include <engine/core/common.hpp>

class Animation;
class Registry;

// idea: starting at the root node, go down the node tree and set the final transform
// code:
// void animateJoint(joint, parentTransform):
//		animTransform = calcAnimTransformForJoint(joint)
//		globalTransform = parentTransform * animTransform
//		joint.finalTransform = globalInverseTransform * globalTransform * joint.baseTransform
//
//		for each childJoint in joint.children:
//			animateJoint(childJoint, globalTransform);

struct Animator {
    Animation* currentAnim = nullptr;
    float animTime = 0.f;
    uint32 frameIndex = 0;
};

void updateAnimators(Registry& registry, float deltaTime);
