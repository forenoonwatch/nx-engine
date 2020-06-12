#pragma once

#include <engine/core/common.hpp>
#include <engine/core/service.hpp>

#include <engine/math/quaternion.hpp>

#include <engine/physics/body.hpp>
#include <engine/physics/collider.hpp>

#include <bullet/btBulletDynamicsCommon.h>

#include <utility>

class Registry;

class PhysicsEngine final : public Service<PhysicsEngine> {
	public:
		PhysicsEngine();

		Body createBody(Collider& collider, float mass,
				const Vector3f& position = Vector3f(0, 0, 0),
				const Quaternion& rotation = Quaternion(1, 0, 0, 0));

		template <typename ColliderType, typename... Args>
		ColliderType createCollider(Args&&... args);

		void step(float deltaTime);

		void writeTransformComponents(Registry& registry);

		void setGravity(const Vector3f& gravity);

		~PhysicsEngine();
	private:
		NULL_COPY_AND_ASSIGN(PhysicsEngine);

		btDefaultCollisionConfiguration collisionConfiguration;
		btCollisionDispatcher dispatcher;
		btDbvtBroadphase broadphase;
		btSequentialImpulseConstraintSolver solver;
		btDiscreteDynamicsWorld dynamicsWorld;

		btAlignedObjectArray<btCollisionShape*> collisionShapes;
};


template <typename ColliderType, typename... Args>
ColliderType PhysicsEngine::createCollider(Args&&... args) {
	ColliderType collider(std::forward<Args>(args)...);
	collisionShapes.push_back(collider.getHandle());

	return collider;
}
