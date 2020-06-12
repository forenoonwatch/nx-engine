#include "engine/physics/physics-engine.hpp"

#include <engine/physics/physics-util.hpp>

#include <engine/ecs/registry.hpp>

#include <engine/components/transform-component.hpp>

#define NUM_SUB_STEPS 10

namespace {
	constexpr const Vector3f DEFAULT_GRAVITY = Vector3f(0, -9.81, 0);
};

PhysicsEngine::PhysicsEngine()
		: dispatcher(&collisionConfiguration)
		, dynamicsWorld(&dispatcher, &broadphase, &solver,
				&collisionConfiguration) {
	setGravity(DEFAULT_GRAVITY);
}

Body PhysicsEngine::createBody(Collider& collider, float mass,
		const Vector3f& position, const Quaternion& rotation) {
	btVector3 localInertia(0.f, 0.f, 0.f);
	btTransform transform;
	transform.setOrigin(Physics::nativeToBtVec3(position));
	transform.setRotation(Physics::nativeToBtQuat(rotation));

	if (mass != 0.f) {
		collider.getHandle()->calculateLocalInertia(mass, localInertia);
	}

	auto* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo hints(mass, motionState,
			collider.getHandle(), localInertia);
	
	auto* btBody = new btRigidBody(hints);
	dynamicsWorld.addRigidBody(btBody);

	return {btBody};
}

void PhysicsEngine::step(float deltaTime) {
	dynamicsWorld.stepSimulation(deltaTime, NUM_SUB_STEPS);
}

void PhysicsEngine::writeTransformComponents(Registry& registry) {
	registry.view<TransformComponent, Body>().each([&](auto& tfc, auto& body) {
		body.getRenderTransform(tfc.transform);
	});
}

void PhysicsEngine::setGravity(const Vector3f& gravity) {
	dynamicsWorld.setGravity(Physics::nativeToBtVec3(gravity));
}

PhysicsEngine::~PhysicsEngine() {
	for (int32 i = dynamicsWorld.getNumCollisionObjects() - 1; i >= 0; --i) {
		auto* obj = dynamicsWorld.getCollisionObjectArray()[i];
		auto* body = btRigidBody::upcast(obj);

		if (body && body->getMotionState()) {
			delete body->getMotionState();
		}

		dynamicsWorld.removeCollisionObject(obj);
		delete obj;
	}

	for (int32 i = 0; i < collisionShapes.size(); ++i) {
		delete collisionShapes[i];
	}
}
