#pragma once

#include <engine/core/common.hpp>
#include <engine/core/service.hpp>
#include <engine/core/hash-map.hpp>

#include <engine/math/quaternion.hpp>

#include <engine/physics/body.hpp>
#include <engine/physics/collider.hpp>
#include <engine/physics/character-controller.hpp>
#include <engine/physics/vehicle-controller.hpp>
#include <engine/physics/debug-renderer.hpp>

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

		CharacterController createCharacterController(Collider& collider,
				float stepHeight, const Vector3f& position = Vector3f(0, 0, 0),
				const Quaternion& rotation = Quaternion(1, 0, 0, 0));

		VehicleController createVehicleController(Collider& collider,
				const Vector3f& position = Vector3f(0, 0, 0),
				const Quaternion& rotation = Quaternion(1, 0, 0, 0));

		void step(float deltaTime);

		void debugDrawWorld();

		void writeTransformComponents(Registry& registry);

		void setGravity(const Vector3f& gravity);

		KinematicVehicleController* findVehicleController(const btGhostObject* ghostObject);

		~PhysicsEngine();
	private:
		NULL_COPY_AND_ASSIGN(PhysicsEngine);

		btDefaultCollisionConfiguration collisionConfiguration;
		btCollisionDispatcher dispatcher;
		btDbvtBroadphase broadphase;
		btSequentialImpulseConstraintSolver solver;
		btDiscreteDynamicsWorld dynamicsWorld;
		PhysicsDebugRenderer debugRenderer;

		btAlignedObjectArray<btCollisionShape*> collisionShapes;

		HashMap<const btGhostObject*, KinematicCharacterController*> charControllers;
		HashMap<const btGhostObject*, KinematicVehicleController*> vehicleControllers;
};

template <typename ColliderType, typename... Args>
ColliderType PhysicsEngine::createCollider(Args&&... args) {
	ColliderType collider(std::forward<Args>(args)...);
	collisionShapes.push_back(collider.getHandle());

	return collider;
}
