#include "engine/physics/physics-engine.hpp"

#include <engine/physics/dynamics/contact-constraint.hpp>
#include <engine/physics/dynamics/island.hpp>
#include <engine/physics/collision/collider.hpp>

#include <engine/game/util-components.hpp>

#include <engine/ecs/ecs.hpp>

Physics::PhysicsEngine::PhysicsEngine()
		: contactManager(*this)
		, newCollider(false) {}

Physics::Body* Physics::PhysicsEngine::addBody(const BodyHints& hints) {
	Body* body = new Body(*this, hints);
	bodies.push_back(body);
	
	return body;
}

void Physics::PhysicsEngine::step(float deltaTime) {
	if (newCollider) {
		newCollider = false;
		contactManager.getBroadphase().updatePairs();
	}

	contactManager.testCollisions();
	
	for (Body* body : bodies) {
		for (Collider* c : body->colliders) {
			c->setWorldTransform(
					body->transform.transform(c->getLocalTransform()));
			//c->setWorldTransform(handle.body->transform);
		}
	}

	for (Body* body : bodies) {
		body->setNotInIsland();
	}

	ArrayList<Body*> bodyStack;

	for (uint32 i = 0; i < bodies.size(); ++i) {
		Body* seed = bodies[i];

		if (seed->isInIsland() || !seed->isAwake() || seed->isStatic()) {
			continue;
		}

		Island island;

		seed->setInIsland();

		bodyStack.clear();
		bodyStack.push_back(seed);

		while (!bodyStack.empty()) {
			Body* body = bodyStack.back();
			bodyStack.pop_back();

			island.add(*body);

			body->setToAwake();

			if (body->isStatic()) {
				continue;
			}

			for (ContactEdge* edge : body->contactList) {
				ContactConstraint* cc = edge->constraint;

				if (cc->isInIsland() || !cc->isColliding()) {
					continue;
				}

				// TODO: skip sensors

				cc->setInIsland();
				island.add(*cc);

				if (edge->other->isInIsland()) {
					continue;
				}

				bodyStack.push_back(edge->other);
				edge->other->setInIsland();
			}
		}

		island.solve(deltaTime);
	}

	contactManager.findNewContacts();

	for (Body* body : bodies) {
		body->force = Vector3f();
		body->torque = Vector3f();
	}

	ECS::Registry::getInstance().view<TransformComponent,
			Physics::BodyHandle>().each([&](TransformComponent& tf,
			Physics::BodyHandle& handle) {
		tf.transform = handle.body->transform;
	});
}

void Physics::PhysicsEngine::addCollider(Body& body, Collider& collider) {
	newCollider = true;
	contactManager.getBroadphase().insert(collider, collider.computeAABB());
}

Physics::PhysicsEngine::~PhysicsEngine() {
	for (Body* body : bodies) {
		delete body;
	}
}

