#include "engine/networking/state-update.hpp"

#include "engine/networking/network-object.hpp"

#include "engine/physics/dynamics/body.hpp"

#include "engine/ecs/registry.hpp"

#include "engine/math/math.hpp"

#include <algorithm>

void serializeBodyStates(StateUpdate& stateUpdate) {
	auto view = ECS::Registry::getInstance().view<NetworkObject,
			Physics::BodyHandle>();
	const size_t numBodies = Math::min(view.size(),
			static_cast<size_t>(StateUpdate::MAX_BODIES));

	uint32 i = 0;
	for (auto it = view.begin(), end = view.end();
			it != end && i < numBodies; ++it) {
		auto& no = view.get<NetworkObject>(*it);
		const auto* body = view.get<Physics::BodyHandle>(*it).body;

		stateUpdate.bodyStates[i].networkID = no.networkID;

		stateUpdate.bodyStates[i].position
				= body->getTransform().getPosition();
		stateUpdate.bodyStates[i].rotation
				= body->getTransform().getRotation();

		if (body->isAwake()) {
			stateUpdate.bodyStates[i].moving = true;
			
			stateUpdate.bodyStates[i].velocity = body->getVelocity();
			stateUpdate.bodyStates[i].angularVelocity
					= body->getAngularVelocity();
		}
		else {
			stateUpdate.bodyStates[i].moving = false;
		}

		no.accumulatedPriority = 0;

		++i;
	}

	stateUpdate.numBodies = i;

	DEBUG_LOG_TEMP("Serialized %d bodies to send", i);
	// TODO: see if this is the optimal way to iterate over the objs
}

void serializeBodyStates(StateUpdate& stateUpdate, uint64 clientID) {
	auto view = ECS::Registry::getInstance().view<NetworkObject,
			Physics::BodyHandle>();
	const size_t numBodies = Math::min(view.size(),
			static_cast<size_t>(StateUpdate::MAX_BODIES));

	uint32 i = 0;
	for (auto it = view.begin(), end = view.end();
			it != end && i < numBodies; ++it) {
		auto& no = view.get<NetworkObject>(*it);
		const auto* body = view.get<Physics::BodyHandle>(*it).body;

		if (no.networkOwner != clientID) {
			break;
		}

		stateUpdate.bodyStates[i].networkID = no.networkID;

		stateUpdate.bodyStates[i].position
				= body->getTransform().getPosition();
		stateUpdate.bodyStates[i].rotation
				= body->getTransform().getRotation();

		if (body->isAwake()) {
			stateUpdate.bodyStates[i].moving = true;
			
			stateUpdate.bodyStates[i].velocity = body->getVelocity();
			stateUpdate.bodyStates[i].angularVelocity
					= body->getAngularVelocity();
		}
		else {
			stateUpdate.bodyStates[i].moving = false;
		}

		no.accumulatedPriority = 0;

		++i;
	}

	stateUpdate.numBodies = i;

	DEBUG_LOG_TEMP("Serialized %d bodies to send", i);
	// TODO: see if this is the optimal way to iterate over the objs
}

void deserializeBodyStates(StateUpdate& stateUpdate) {
	ECS::Registry::getInstance().sort<NetworkObject>([](const auto& a,
			const auto& b) {
		return a.networkID < b.networkID;
	});

	std::sort(stateUpdate.bodyStates, stateUpdate.bodyStates
			+ stateUpdate.numBodies, [](const auto& a, const auto& b) {
		return a.networkID < b.networkID;
	});
	
	auto view = ECS::Registry::getInstance().view<NetworkObject,
			Physics::BodyHandle>();
	auto it = view.begin();
	auto end = view.end();

	DEBUG_LOG_TEMP("Got %d bodies from remote", stateUpdate.numBodies);

	for (uint32 i = 0; i < stateUpdate.numBodies; ++i) {
		const auto& bodyState = stateUpdate.bodyStates[i];

		while (it != end) {
			auto& no = view.get<NetworkObject>(*it);

			if (no.networkID >= bodyState.networkID) {
				break;
			}
			
			++it;
		}

		if (it != end) {
			auto& no = view.get<NetworkObject>(*it);

			if (no.networkID == bodyState.networkID) {
				auto* body = view.get<Physics::BodyHandle>(*it).body;

				body->setTransform(Transform(bodyState.position,
						bodyState.rotation, Vector3f(1.f)));

				if (bodyState.moving) {
					body->setVelocity(bodyState.velocity);
					body->setAngularVelocity(bodyState.angularVelocity);

					body->setToAwake();
				}
			}
		}
	}
}

