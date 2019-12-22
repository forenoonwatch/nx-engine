#pragma once

#include <yojimbo/yojimbo.h>

#include <engine/core/common.hpp>

#include <engine/math/vector.hpp>
#include <engine/math/quaternion.hpp>

struct InputState {
	enum {
		INPUT_FORWARD = 1,
		INPUT_BACK = 2,
		INPUT_LEFT = 4,
		INPUT_RIGHT = 8
	};

	uint32 data;
};

struct BodyState {
	Vector3f position;
	Quaternion rotation;

	bool moving; // omit velocity and angular velocity if asleep/static/etc

	Vector3f velocity;
	Vector3f angularVelocity;

	template <typename Stream>
	bool Serialize(Stream& stream) {
		serialize_bytes(stream, (uint8*)&position, sizeof(Vector3f));
		serialize_bytes(stream, (uint8*)&rotation, sizeof(Quaternion));

		serialize_bool(stream, moving);

		if (moving) {
			serialize_bytes(stream, (uint8*)&velocity, sizeof(Vector3f));
			serialize_bytes(stream, (uint8*)&angularVelocity,
					sizeof(Vector3f));
		}

		return true;
	}
};

struct StateUpdate {
	static constexpr const uint32 MAX_BODIES = 5;

	InputState inputState;

	uint32 numBodies;

	uint32 networkIDs[MAX_BODIES];
	BodyState bodyStates[MAX_BODIES];

	uint16 sequence;
};

