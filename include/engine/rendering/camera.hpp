#pragma once

#include <engine/math/matrix.hpp>

// TODO: encapsulate
struct Camera {
	Matrix4f projection;
	Matrix4f view;
	Matrix4f viewProjection;
	
	Matrix4f iView;
	Matrix4f iViewProjection;
	Matrix4f iProjection;

	inline Vector3f getRightVector() const { return Vector3f(view[0]); }
	inline Vector3f getUpVector() const { return Vector3f(view[1]); }
	inline Vector3f getLookVector() const { return Vector3f(view[2]); }
	inline Vector3f getPosition() const { return Vector3f(view[3]); }

	void update();
};
