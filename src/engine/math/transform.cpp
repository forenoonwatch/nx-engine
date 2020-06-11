#include "engine/math/transform.hpp"

#include <engine/math/math.hpp>

Matrix4f Transform::inverse() const {
	return Math::inverse(toMatrix());
}

Transform& Transform::mix(const Transform& dest, float amt,
		Transform& result) const {
	result.setPosition(Math::mix(position, dest.getPosition(), amt));
	result.setRotation(Math::slerp(rotation, dest.getRotation(), amt));
	result.setScale(Math::mix(scale, dest.getScale(), amt));

	return result;
}

void Transform::quatLookAt(const Vector3f& target, const Vector3f& axis) {
	const Vector3f diff = Math::normalize(target - position);
	const Vector3f a = Math::normalize(Math::cross(axis, diff));

	float c = Math::dot(diff, axis);

	// TODO: figure out what to do if they're parallel

	float halfC = Math::sqrt(0.5f + 0.5f * c);
	float halfS = Math::sqrt(0.5f - 0.5f * c);

	rotation = Quaternion(halfC, a.x * halfS, a.y * halfS, a.z * halfS);
}

void Transform::matrixLookAt(const Vector3f& target) {
	constexpr const Vector3f UP(0, 1, 0);

	const Vector3f diff = Math::normalize(target - position);
	Vector3f right = Math::normalize(Math::cross(UP, diff));
	Vector3f up = Math::cross(diff, right);

	rotation = Math::mat3ToQuat(Matrix3f(right, up, diff));
}
