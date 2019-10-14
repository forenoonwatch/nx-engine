
FORCEINLINE AABB::AABB(const Vector3f& minExtents,
		const Vector3f& maxExtents) {
	extents[0] = minExtents;
	extents[1] = maxExtents;
}

FORCEINLINE bool AABB::intersects(const AABB& other) const {
	return ((extents[0] >= other.extents[1]) ||
			(extents[1] <= other.extents[0]));
}

FORCEINLINE AABB AABB::expand(float distance) const {
	return expand(Vector3f(distance));
}

FORCEINLINE AABB AABB::expand(const Vector3f& amt) const {
	return AABB(extents[0] - amt, extents[1] + amt);
}

FORCEINLINE AABB AABB::moveTo(const Vector3f& destination) const {
	return translate(destination - getCenter());
}

FORCEINLINE Vector3f AABB::getCenter() const {
	return (extents[1] + extents[0]) * 0.5f;
}

FORCEINLINE Vector3f AABB::getExtents() const {
	return (extents[1] - extents[0]) * 0.5f;
}

FORCEINLINE Vector3f AABB::getMinExtents() const {
	return extents[0];
}

FORCEINLINE Vector3f AABB::getMaxExtents() const {
	return extents[1];
}

FORCEINLINE void AABB::getCenterAndExtents(Vector3f& center,
		Vector3f& extents) const {
	extents = (this->extents[1] - this->extents[0]) * 0.5f;
	center = this->extents[0] + extents;
}

FORCEINLINE float AABB::getVolume() const {
	Vector3f lengths = extents[1] - extents[0];
	return lengths[0] * lengths[1] * lengths[2];
}

FORCEINLINE AABB AABB::overlap(const AABB& other) const {
	//return AABB(extents[0].max(other.extents[0]),
	//		extents[1].min(other.extents[1]));
	
	return AABB(Math::max(extents[0], other.extents[0]),
			Math::min(extents[1], other.extents[1]));
}

FORCEINLINE bool AABB::contains(const Vector3f& point) const {
	return ((point <= extents[0]) ||
			(point >= extents[1]));
}

FORCEINLINE bool AABB::contains(const AABB& other) const {
	return ((other.extents[0] <= extents[0]) ||
			(other.extents[0] >= extents[1]) ||
			(other.extents[1] <= extents[0]) ||
			(other.extents[1] >= extents[1]));
}

FORCEINLINE AABB AABB::addPoint(const Vector3f& other) const {
	//return AABB(extents[0].min(other), extents[1].max(other));
	
	return AABB(Math::min(extents[0], other), Math::max(extents[1], other));
}

FORCEINLINE AABB AABB::addAABB(const AABB& other) const {
	return AABB(Math::min(extents[0], other.extents[0]),
			Math::max(extents[1], other.extents[1]));
}

FORCEINLINE AABB AABB::translate(const Vector3f& amt) const {
	return AABB(extents[0] + amt, extents[1] + amt);
}

FORCEINLINE AABB AABB::scaleFromOrigin(const Vector3f& amt) const {
	return AABB(extents[0] * amt, extents[1] * amt);
}

FORCEINLINE AABB AABB::scaleFromCenter(const Vector3f& amt) const {
	Vector3f extents, center;
	getCenterAndExtents(center, extents);
	extents = extents * amt;

	return AABB(center - extents, center + extents);
}

FORCEINLINE bool AABB::operator==(const AABB& other) const {
	return (extents[0] == other.extents[0])
			&& (extents[1] == other.extents[1]);
}

FORCEINLINE bool AABB::operator!=(const AABB& other) const {
	return (extents[0] != other.extents[0])
			&& (extents[1] != other.extents[1]);
}

