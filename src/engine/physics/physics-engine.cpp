#include "engine/physics/physics-engine.hpp"

#include <engine/physics/physics-util.hpp>
#include <engine/physics/character-controller.hpp>
#include <engine/physics/vehicle-controller.hpp>

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

	dynamicsWorld.setDebugDrawer(&debugRenderer);
	debugRenderer.setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	dynamicsWorld.getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
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

CharacterController* PhysicsEngine::createCharacterController(Collider& collider,
		const Vector3f& position, const Quaternion& rotation) {
	auto* ghostObject = new btPairCachingGhostObject();
	ghostObject->setWorldTransform(btTransform(Physics::nativeToBtQuat(rotation),
			Physics::nativeToBtVec3(position)));

	ghostObject->setCollisionShape(collider.getHandle());
	ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	auto* controller = new CharacterController(ghostObject,
			static_cast<btConvexShape*>(collider.getHandle()));
	//controller->setGravity(dynamicsWorld.getGravity());

	dynamicsWorld.addCollisionObject(ghostObject, btBroadphaseProxy::CharacterFilter,
			btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
	dynamicsWorld.addAction(controller);

	//controller->reset(&dynamicsWorld);

	actionInterfaces[ghostObject] = controller;

	return controller;
}

VehicleController* PhysicsEngine::createVehicleController(Collider& collider,
		const Vector3f& position, const Quaternion& rotation) {
	auto* ghostObject = new btPairCachingGhostObject();
	ghostObject->setWorldTransform(btTransform(Physics::nativeToBtQuat(rotation),
			Physics::nativeToBtVec3(position)));
	
	ghostObject->setCollisionShape(collider.getHandle());
	ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT); // TODO: see what this does

	auto* controller = new VehicleController(ghostObject, collider.getHandle());

	dynamicsWorld.addCollisionObject(ghostObject);
	//dynamicsWorld.addCollisionObject(ghostObject, btBroadphaseProxy::CharacterFilter,
	//		btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
	dynamicsWorld.addAction(controller);

	actionInterfaces[ghostObject] = controller;

	return controller;
}

void PhysicsEngine::step(float deltaTime) {
	dynamicsWorld.stepSimulation(deltaTime, NUM_SUB_STEPS);
}

void PhysicsEngine::debugDrawWorld() {
	dynamicsWorld.debugDrawWorld();
	debugRenderer.flush();
}

void PhysicsEngine::writeTransformComponents(Registry& registry) {
	registry.view<TransformComponent, Body>().each([&](auto& tfc, auto& body) {
		body.getRenderTransform(tfc.transform);
	});
}

void PhysicsEngine::setGravity(const Vector3f& gravity) {
	dynamicsWorld.setGravity(Physics::nativeToBtVec3(gravity));
}

btActionInterface* PhysicsEngine::findActionInterface(const btGhostObject* ghostObject) {
	if (auto it = actionInterfaces.find(ghostObject);
			it != actionInterfaces.end()) {
		return it->second;
	}

	return nullptr;
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
