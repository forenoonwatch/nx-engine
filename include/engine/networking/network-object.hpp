#pragma once

#include <engine/core/common.hpp>

enum class NetworkRole {
	ROLE_NONE,
	ROLE_SYNCHRONIZED,
	ROLE_AUTONOMOUS,
	ROLE_AUTHORITATIVE,

	NUM_ROLES
};

struct NetworkObject {
	uint64 networkOwner; // TODO: replace with wrapper for a client
	uint32 networkID; // TODO: match this to entt::entity
	uint32 networkPriority;
	
	uint32 accumulatedPriority;

	NetworkRole role;
	NetworkRole remoteRole;

	inline bool operator<(const NetworkObject& n) const {
		return accumulatedPriority < n.accumulatedPriority;
	}
};

