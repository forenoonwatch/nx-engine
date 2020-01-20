#pragma once

#include <engine/networking/state-update.hpp>

#include <yojimbo/yojimbo.h>

struct StateUpdateMessage final : public yojimbo::Message {
	StateUpdate stateUpdate;

	template <typename Stream>
	bool Serialize(Stream& stream) {
		serialize_bytes(stream, (uint8*)&stateUpdate.inputState,
				sizeof(InputState));

		serialize_bits(stream, stateUpdate.numBodies, 32);

		for (uint32 i = 0; i < stateUpdate.numBodies; ++i) {
			serialize_object(stream, stateUpdate.bodyStates[i]);
		}

		return true;
	}

	YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

