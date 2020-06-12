#pragma once

#include <engine/core/common.hpp>

#include <engine/math/matrix.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/math/transform.hpp>

#include <bullet/LinearMath/btVector3.h>
#include <bullet/LinearMath/btQuaternion.h>
#include <bullet/LinearMath/btTransform.h>

namespace Physics {
    FORCEINLINE Vector3f btToNativeVec3(const btVector3& v) {
        return Vector3f(v.x(), v.y(), v.z());
    }

    FORCEINLINE btVector3 nativeToBtVec3(const Vector3f& v) {
        return btVector3(v.x, v.y, v.z);
    }

    FORCEINLINE Quaternion btToNativeQuat(const btQuaternion& q) {
        return Quaternion(q.w(), q.x(), q.y(), q.z());
    }

    FORCEINLINE btQuaternion nativeToBtQuat(const Quaternion& q) {
        return btQuaternion(q.x, q.y, q.z, q.w);
    }

    FORCEINLINE void btToNativeTransform(Transform& dest, const btTransform& src) {
        dest.setPosition(btToNativeVec3(src.getOrigin()));
        dest.setRotation(btToNativeQuat(src.getRotation()));
    }

    FORCEINLINE void nativeToBtTransform(btTransform& dest, const Transform& src) {
        dest.setOrigin(nativeToBtVec3(src.getPosition()));
        dest.setRotation(nativeToBtQuat(src.getRotation()));
    }

    FORCEINLINE void btTransformToMat4(Matrix4f& dest, const btTransform& src) {
        src.getOpenGLMatrix(reinterpret_cast<btScalar*>(&dest));
    }
};
