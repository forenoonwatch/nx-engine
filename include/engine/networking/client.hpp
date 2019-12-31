#pragma once

#include <engine/networking/net-common.hpp>
#include <engine/networking/network-object.hpp>

#include <engine/core/common.hpp>
#include <engine/core/memory.hpp>
#include <engine/core/array-list.hpp>
#include <engine/core/singleton.hpp>

class NetworkClient final : public Singleton<NetworkClient>,
		public NetworkInitializer {
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

