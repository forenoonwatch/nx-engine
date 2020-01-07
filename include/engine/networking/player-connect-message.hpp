#pragma once

#include <yojimbo/yojimbo.h>

struct PlayerConnectMessage final : public yojimbo::Message {
	uint64 clientID;
	uint32 networkID;

	template <typename Stream>	
	bool Serialize(Stream& stream) {
		serialize_uint64(stream, clientID);
		serialize_uint32(stream, networkID);

		return true;
	}

	YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

