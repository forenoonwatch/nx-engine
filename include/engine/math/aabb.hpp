#pragma once

#include <engine/math/vector.hpp>
#include <engine/math/matrix.hpp>

class AABB {
	public:
		FORCEINLINE AABB() {}
		FORCEINLINE AABB(const Vector3f& minExtents,
				const Vector3f& maxExtents);
		
		AABB(Vector3f* points, uint32 amt);
		AABB(float* points, uint32 amt, uint32 stride = 0);

		bool intersectRay(const Vector3f& start,
				const Vector3f& rayDir, float& point1, float& point2) const;
		bool intersectLine(const Vector3f& start,
				const Vector3f& end) const;
		FORCEINLINE bool intersects(const AABB& other) const;
		AABB transform(const Matrix4f& transform) const;

		FORCEINLINE AABB expand(float distance) const;
		FORCEINLINE AABB expand(const Vector3f& amt) const;
		FORCEINLINE AABB moveTo(const Vector3f& destination) const;
		
		FORCEINLINE Vector3f getCenter() const;
		FORCEINLINE Vector3f getExtents() const;
		FORCEINLINE Vector3f getMinExtents() const;
		FORCEINLINE Vector3f getMaxExtents() const;

		FORCEINLINE void getCenterAndExtents(Vector3f& center,
				Vector3f& extents) const;
		FORCEINLINE float getVolume() const;

		FORCEINLINE AABB overlap(const AABB& other) const;
		
		FORCEINLINE bool contains(const Vector3f& point) const;

		FORCEINLINE AABB translate(const Vector3f& amt) const;
		
		FORCEINLINE AABB scaleFromCenter(const Vector3f& amt) const;
		FORCEINLINE AABB scaleFromOrigin(const Vector3f& amt) const;

		FORCEINLINE AABB addPoint(const Vector3f& other) const;
		FORCEINLINE AABB addAABB(const AABB& other) const;

		FORCEINLINE bool operator==(const AABB& other) const;
		FORCEINLINE bool operator!=(const AABB& other) const;
	private:
		Vector3f extents[2];
};

#include "engine/math/aabb.inl"

