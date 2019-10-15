#pragma once

#include <engine/math/matrix.hpp>

#include <GLM/gtc/quaternion.hpp>

#define Quaternion glm::quat

namespace Math {
	FORCEINLINE Quaternion conjugate(const Quaternion& q) {
		return glm::conjugate(q);
	}

	FORCEINLINE Quaternion rotate(const Quaternion& q, float angle,
			const Vector3f& axis) {
		return glm::rotate(q, angle, axis);
	}

	FORCEINLINE Matrix4f quatToMat4(const Quaternion& q) {
		return glm::mat4_cast(q);
	}

	FORCEINLINE Quaternion mat4ToQuad(const Matrix4f& mat) {
		return glm::quat_cast(mat);
	}
};

