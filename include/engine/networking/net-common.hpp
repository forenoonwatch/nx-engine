#pragma once

#include <engine/networking/net-message.hpp>
#include <engine/networking/net-adapter.hpp>

#define PROTOCOL_ID 0x11223344556677ULL

enum class GameChannelType {
	UNRELIABLE = 0,
	RELIABLE,

	NUM_CHANNELS
};

struct GameConnectionConfig : public yojimbo::ClientServerConfig {
	GameConnectionConfig() {
		protocolId = PROTOCOL_ID;
		networkSimulator = false;

		numChannels = (int)GameChannelType::NUM_CHANNELS;

		channel[(int)GameChannelType::UNRELIABLE].type
				= yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED;
		channel[(int)GameChannelType::RELIABLE].type
				= yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED;
	}
};

class NetworkInitializer {
	public:
		NetworkInitializer() {
			if (!initialized) {
				initialized = true;

				InitializeYojimbo();
				//yojimbo_log_level(YOJIMBO_LOG_LEVEL_INFO);
				yojimbo_log_level(YOJIMBO_LOG_LEVEL_DEBUG);
			}
		}

		virtual ~NetworkInitializer() {
			if (initialized) {
				initialized = false;

				ShutdownYojimbo();
			}
		}
	private:
		static inline bool initialized = false;
};

