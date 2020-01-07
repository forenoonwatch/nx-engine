#pragma once

#include <yojimbo/yojimbo.h>

#include <engine/core/common.hpp>

struct InputState {
	enum {
		INPUT_FORWARD = 1,
		INPUT_BACK = 2,
		INPUT_LEFT = 4,
		INPUT_RIGHT = 8,
		INPUT_JUMP = 16,
		INPUT_LEFT_MOUSE = 32,
		INPUT_RIGHT_MOUSE = 64
	};

	// TODO: represent owner IDs
	uint32 inputs;
	float mouseDeltaX;
	float mouseDeltaY;

	template <typename Stream>
	bool Serialize(Stream& stream) {
		serialize_uint32(stream, inputs);

		serialize_float(stream, mouseDeltaX);
		serialize_float(stream, mouseDeltaY);
	}
};

void writeInputStates(InputState* inputStates);

