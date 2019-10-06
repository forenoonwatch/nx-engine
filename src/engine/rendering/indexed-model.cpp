#include "engine/rendering/indexed-model.hpp"

#include "engine/math/intersects.hpp"

#include <cfloat>

IndexedModel::IndexedModel(const AllocationHints& hints)
		: instancedElementStartIndex(hints.instancedElementStartIndex)
		, flags(hints.flags) {
	elementSizes.assign(hints.elementSizes.begin(), hints.elementSizes.end());
	elements.resize(hints.elementSizes.size());
}

bool IndexedModel::intersectsRay(const Vector3f& pos, const Vector3f& dir,
		Vector3f* intersectPos, Vector3f* normal) const {
	uint32 i0, i1, i2;
	uint32 minI0, minI1, minI2;

	float t;
	float minT = FLT_MAX;
	bool intersected = false;
	
	for (uint32 i = 0; i < indices.size(); i += 3) {
		i0 = indices[i];
		i1 = indices[i + 1];
		i2 = indices[i + 2];

		/*Vector3f v0 = Vector3f(&elements[0][3 * i0]);
		Vector3f v1 = Vector3f(&elements[0][3 * i1]);
		Vector3f v2 = Vector3f(&elements[0][3 * i2]);*/

		const Vector3f& v0 = *((Vector3f*)&elements[0][3 * i0]);
		const Vector3f& v1 = *((Vector3f*)&elements[0][3 * i1]);
		const Vector3f& v2 = *((Vector3f*)&elements[0][3 * i2]);

		if (Intersects::intersectTriangle(pos, dir, v0, v1, v2, t)
				&& t < minT) {
			minT = t;
			intersected = true;

			minI0 = i0;
			minI1 = i1;
			minI2 = i2;
		}
	}

	if (intersected) {
		*intersectPos = pos + dir * minT;

		const Vector3f& n0 = *((Vector3f*)&elements[2][3 * minI0]);
		const Vector3f& n1 = *((Vector3f*)&elements[2][3 * minI1]);
		const Vector3f& n2 = *((Vector3f*)&elements[2][3 * minI2]);

		*normal = (n0 + n1 + n2) / 3.f;
	}

	return intersected;
}

void IndexedModel::initStaticMesh() {
	allocateElement(3); // Positions
	allocateElement(2); // TexCoords
	allocateElement(3); // Normals
	allocateElement(3); // Tangents
	allocateElement(2 * 16); // MVP, model matrix

	setInstancedElementStartIndex(4); // Begin instanced data
}

void IndexedModel::addElement1f(uint32 elementIndex, float e0) {
	elements[elementIndex].push_back(e0);
}

void IndexedModel::addElement2f(uint32 elementIndex, float e0, float e1) {
	elements[elementIndex].push_back(e0);
	elements[elementIndex].push_back(e1);
}

void IndexedModel::addElement3f(uint32 elementIndex, float e0, float e1, float e2) {
	elements[elementIndex].push_back(e0);
	elements[elementIndex].push_back(e1);
	elements[elementIndex].push_back(e2);
}

void IndexedModel::addElement4f(uint32 elementIndex, float e0, float e1, float e2, float e3) {
	elements[elementIndex].push_back(e0);
	elements[elementIndex].push_back(e1);
	elements[elementIndex].push_back(e2);
	elements[elementIndex].push_back(e3);
}

void IndexedModel::setElement4f(uint32 elementIndex, uint32 arrayIndex,
		float e0, float e1, float e2, float e3) {
	arrayIndex *= elementSizes[elementIndex];

	elements[elementIndex][arrayIndex] = e0;
	elements[elementIndex][arrayIndex + 1] = e1;
	elements[elementIndex][arrayIndex + 2] = e2;
	elements[elementIndex][arrayIndex + 3] = e3;
}

void IndexedModel::addIndices1i(uint32 i0) {
	indices.push_back(i0);
}

void IndexedModel::addIndices2i(uint32 i0, uint32 i1) {
	indices.push_back(i0);
	indices.push_back(i1);
}

void IndexedModel::addIndices3i(uint32 i0, uint32 i1, uint32 i2) {
	indices.push_back(i0);
	indices.push_back(i1);
	indices.push_back(i2);
}

void IndexedModel::addIndices4i(uint32 i0, uint32 i1, uint32 i2, uint32 i3) {
	indices.push_back(i0);
	indices.push_back(i1);
	indices.push_back(i2);
	indices.push_back(i3);
}

