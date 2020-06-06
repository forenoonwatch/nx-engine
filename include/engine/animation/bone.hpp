#pragma once

#include <engine/core/common.hpp>
#include <engine/core/string.hpp>
#include <engine/core/array-list.hpp>

#include <engine/math/matrix.hpp>

struct Bone {
	inline Bone(const Matrix4f& localTransform,
            const String& name, uint32 index)
		: localTransform(localTransform)
		, name(name)
		, index(index)
		, parent(-1) {}

	Matrix4f localTransform;
	String name;
	uint32 index;
	uint32 parent;

	ArrayList<uint32> children;
};
