#pragma once

#include <engine/networking/net-common.hpp>

#include <engine/core/common.hpp>
#include <engine/core/singleton.hpp>

class NetworkClient final : public Singleton<NetworkClient>,
		public NetworkInitializer {
	public:
		typedef void (*MessageCallback)(yojimbo::Message*);

		NetworkClient();

		void connect(const char* serverAddress, uint32 serverPort,
				const uint8* privateKey);

		void receiveMessages();
		void sendMessages();

		void disconnect();

		inline void setMessageCallback(MessageCallback callback) {
			recvCallback = callback;
		}

		bool isConnected() const;
		bool canSendMessage(enum GameChannelType channel) const;

		inline uint64 getClientID() const { return client.GetClientId(); } 
		inline double getTime() const { return client.GetTime(); }

		template <typename Message_, typename System>
		inline void sendMessageWithSystem(enum GameMessageType messageID,
				enum GameChannelType channel, const System& system);
	private:
		GameAdapter adapter;
		GameConnectionConfig config;

		yojimbo::Client client;

		MessageCallback recvCallback;
		
		yojimbo::Address serverAddress;
};

template <typename Message_, typename System>
inline void NetworkClient::sendMessageWithSystem(
		enum GameMessageType messageID, enum GameChannelType channel,
		const System& system) {
	Message_* msg = static_cast<Message_*>(client.CreateMessage(
			static_cast<int>(messageID)));

	system(msg);

	client.SendMessage(static_cast<int>(channel), msg);
}

