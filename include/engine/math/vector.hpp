#pragma once

#include "engine/core/common.hpp"

#include <GLM/glm.hpp>

#define Vector2f glm::vec2
#define Vector3f glm::vec3
#define Vector4f glm::vec4

namespace Math {
	template <typename T>
	FORCEINLINE float length(const T& vector) {
		return glm::length(vector);
	}

	template <typename T>
	FORCEINLINE float dot(const T& a, const T& b) {
		return glm::dot(a, b);
	}

	FORCEINLINE Vector3f cross(const Vector3f& a, const Vector3f& b) {
		return glm::cross(a, b);
	}
};