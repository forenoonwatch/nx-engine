#include "engine/components/animator.hpp"

#include <engine/animation/animation.hpp>
#include <engine/animation/rig.hpp>

#include <engine/components/rigged-mesh.hpp>

#include <engine/ecs/registry.hpp>

namespace {
	void calcJointTransform(Rig& rig, KeyFrame& frame0, KeyFrame& frame1,
			float lerpAmt, Bone* bone, const Matrix4f& parentTransform);
};

void updateAnimators(Registry& registry, float deltaTime) {
	registry.view<Animator, RiggedMesh>().each([&](auto& animator, auto& mesh) {
		if (animator.currentAnim) {
			auto& rig = *mesh.rig;

			animator.animTime += deltaTime;
			KeyFrame& currentFrame = animator.currentAnim->getKeyFrame(animator.frameIndex);
			KeyFrame& nextFrame = animator.currentAnim->getKeyFrame(animator.frameIndex + 1);

			float lerpAmt = (animator.animTime - currentFrame.getTime())
					/ (nextFrame.getTime() - currentFrame.getTime());

			::calcJointTransform(rig, currentFrame, nextFrame,
					lerpAmt, rig.getRootBone(), Matrix4f(1.f));

			if (animator.animTime >= nextFrame.getTime()) {
				++animator.frameIndex;

				if (animator.frameIndex == animator.currentAnim->getNumFrames() - 1) {
					animator.frameIndex = 0;
					animator.animTime -= nextFrame.getTime();
				}
			}
		}
	});
}

namespace {
	void calcJointTransform(Rig& rig, KeyFrame& frame0, KeyFrame& frame1,
			float lerpAmt, Bone* bone, const Matrix4f& parentTransform) {
		Transform res;

		frame0.getBoneTransform(bone->name).mix(frame1.getBoneTransform(bone->name), lerpAmt, res);
		
		Matrix4f globalTransform = parentTransform * res.toMatrix();

		rig.setFinalTransform(bone->index,
				rig.getGlobalInverseTransform() * globalTransform * bone->localTransform);

		for (uint32 i = 0; i < bone->children.size(); ++i) {
			::calcJointTransform(rig, frame0, frame1, lerpAmt,
					&rig.getBone(bone->children[i]), globalTransform);
		}
	}
};
