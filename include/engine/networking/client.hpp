#pragma once

#include <engine/networking/net-common.hpp>

#include <engine/core/common.hpp>
#include <engine/core/array-list.hpp>
#include <engine/core/singleton.hpp>

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

class NetworkClient final : public Singleton<NetworkClient> {
	public:
		NetworkClient();

		void connect(const char* serverAddress, uint32 serverPort,
				const uint8* privateKey);

		void receiveMessages();
		void sendMessages();

		void disconnect();

		bool isConnected() const;
	
		void addInputState(const InputState& is);	
		void addNetworkObject(const NetworkObject& netObj);
	private:
		GameAdapter adapter;
		GameConnectionConfig config;

		yojimbo::Client client;
		
		yojimbo::Address serverAddress;

		InputState inputState;
		ArrayList<NetworkObject> priorityBuffer;

		void processMessage(yojimbo::Message*);

		void accumulatePriorities();
};

