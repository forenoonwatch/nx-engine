#include "engine/networking/state-update.hpp"

#include "engine/networking/network-object.hpp"

#include "engine/physics/dynamics/body.hpp"

#include "engine/ecs/registry.hpp"

#include "engine/math/math.hpp"

// TODO: see if this doesn't need to be moved to a more general place
// TODO: see how to separate checking client ID
void accumulatePriorities() {
	ECS::Registry::getInstance().view<NetworkObject>().each([](auto& no) {
		no.accumulatedPriority += no.networkPriority;
	});

	ECS::Registry::getInstance().sort<NetworkObject>([](const auto& a,
			const auto& b) {
		return a.accumulatedPriority < b.accumulatedPriority;
	});
}

void serializeBodyStates(StateUpdate& stateUpdate) {
	accumulatePriorities();

	auto view = ECS::Registry::getInstance().view<NetworkObject,
			Physics::BodyHandle>();
	const size_t numBodies = Math::min(view.size(),
			static_cast<size_t>(StateUpdate::MAX_BODIES));

	uint32 i = 0;
	for (auto it = view.begin(), end = view.end();
			it != end && i < numBodies; ++it) {
		auto& no = view.get<NetworkObject>(*it);
		const auto* body = view.get<Physics::BodyHandle>(*it).body;

		stateUpdate.networkIDs[i] = no.networkID;

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
	}
	// TODO: see if this is the optimal way to iterate over the objs
}

void deserializeBodyStates(const StateUpdate& stateUpdate) {
	
	// TODO: figure out how to deserialize
}

