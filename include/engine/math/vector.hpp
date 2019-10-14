#pragma once

#include "engine/core/common.hpp"

#include <GLM/glm.hpp>

#define Vector2f glm::vec2
#define Vector3f glm::vec3
#define Vector4f glm::vec4

#define Vector2i glm::ivec2
#define Vector3i glm::ivec3
#define Vector4i glm::ivec4

namespace Math {
	template <typename T>
	FORCEINLINE float length(const T& vector) {
		return glm::length(vector);
	}

	template <typename T>
	FORCEINLINE float dot(const T& a, const T& b) {
		return glm::dot(a, b);
	}

	template <typename T>
	FORCEINLINE T normalize(const T& v) {
		return glm::normalize(v);
	}

	template <typename T>
	FORCEINLINE T floor(const T& v) {
		return glm::floor(v);
	}

	template <typename T>
	FORCEINLINE T ceil(const T& v) {
		return glm::ceil(v);
	}

	template <typename T>
	FORCEINLINE float min(const T& v) {
		return glm::min(v);
	}

	template <typename T>
	FORCEINLINE float max(const T& v) {
		return glm::max(v);
	}

	template <typename T>
	FORCEINLINE T min(const T& a, const T& b) {
		return glm::min(a, b);
	}

	template <typename T>
	FORCEINLINE T max(const T& a, const T& b) {
		return glm::max(a, b);
	}

	FORCEINLINE Vector3f cross(const Vector3f& a, const Vector3f& b) {
		return glm::cross(a, b);
	}
};

bool operator<=(const Vector3f& a, const Vector3f& b) {
	return a.x <= b.x && a.y <= b.y && a.z <= b.z;
}

bool operator>=(const Vector3f& a, const Vector3f& b) {
	return a.x >= b.x && a.y >= b.y && a.z >= b.z;
}

