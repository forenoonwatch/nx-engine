#pragma once

#include <engine/math/transform.hpp>

namespace Math { 
	void distanceGJK(const Vector3f* verticesA, uint32 numVerticesA,
			const Vector3f* verticesB, uint32 numVerticesB,
			const Transform& tfA, const Transform& tfB, Vector3f& outA,
			Vector3f& outB);
};

