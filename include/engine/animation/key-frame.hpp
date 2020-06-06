#pragma once

#include <engine/core/string.hpp>
#include <engine/core/hash-map.hpp>

#include <engine/math/transform.hpp>

class KeyFrame {
	public:
		inline KeyFrame(float time)
			: time(time) {}

		//inline KeyFrame(const KeyFrame& keyFrame)
		//	: time(keyFrame.time)
		//	, boneTransforms(std::move(keyFrame.boneTransforms)) {}

		inline float getTime() const { return time; }

		inline void addBoneTransform(const String& name, const Transform& transform) {
			boneTransforms[name] = transform;
		}

		inline const Transform& getBoneTransform(const String& name) { return boneTransforms[name]; }
	private:
		float time;
		HashMap<String, Transform> boneTransforms;
};
