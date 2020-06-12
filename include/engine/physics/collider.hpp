#pragma once

#include <engine/math/vector.hpp>

#include <bullet/BulletCollision/CollisionShapes/btCollisionShape.h>
#include <bullet/BulletCollision/BroadphaseCollision/btBroadphaseProxy.h>

enum class ColliderType {
	BOX = BOX_SHAPE_PROXYTYPE,
	CAPSULE = CAPSULE_SHAPE_PROXYTYPE,
};

class Collider {
	public:
		inline btCollisionShape* getHandle() { return handle; }
		inline ColliderType getType() { return type; }
	protected:
		inline Collider(btCollisionShape* handle, enum ColliderType type)
				: handle(handle)
				, type(type) {}
	private:
		btCollisionShape* handle;
		enum ColliderType type;

		friend class PhysicsEngine;
};

class BoxCollider final : public Collider {
	public:
		BoxCollider(const Vector3f& halfSize);
};

class CapsuleCollider final : public Collider {
	public:
		CapsuleCollider(float radius, float height);
};
