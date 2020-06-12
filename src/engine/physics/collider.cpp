#include "engine/physics/collider.hpp"

#include <engine/physics/physics-util.hpp>

#include <bullet/BulletCollision/CollisionShapes/btBoxShape.h>
#include <bullet/BulletCollision/CollisionShapes/btCapsuleShape.h>

BoxCollider::BoxCollider(const Vector3f& halfSize)
	: Collider(new btBoxShape(Physics::nativeToBtVec3(halfSize)), ColliderType::BOX) {}

CapsuleCollider::CapsuleCollider(float radius, float height)
	: Collider(new btCapsuleShape(radius, height), ColliderType::CAPSULE) {}