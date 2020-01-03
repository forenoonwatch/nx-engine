#pragma once

#include <engine/core/common.hpp>

#include <engine/ecs/ecs-fwd.hpp>

enum class NetworkRole {
	ROLE_NONE,
	ROLE_SYNCHRONIZED,
	ROLE_AUTONOMOUS,
	ROLE_AUTHORITATIVE,

	NUM_ROLES
};

struct NetworkObject {
	uint64 networkOwner; // TODO: replace with wrapper for a client
	uint32 networkID; 
	uint32 networkPriority;

	uint32 accumulatedPriority;

	NetworkRole role;
	NetworkRole remoteRole;
};

