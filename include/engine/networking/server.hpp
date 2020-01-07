#pragma once

#include <engine/core/common.hpp>
#include <engine/core/memory.hpp>
#include <engine/core/singleton.hpp>

#include <engine/core/array-list.hpp>

#include <engine/networking/net-common.hpp>
#include <engine/networking/client-connection.hpp>

class NetworkServer final : public Singleton<NetworkServer>,
		public NetworkInitializer {
	public:
		typedef void (*MessageCallback)(ClientConnection&, yojimbo::Message*);
		typedef void (*ConnectionCallback)(ClientConnection&);

		NetworkServer();

		void start(const char* address, uint32 port, const uint8* privateKey);

		void receiveMessages();
		void sendMessages();

		void stop();

		void onClientConnected(uint32 clientIndex);
		void onClientDisconnected(uint32 clientIndex);

		bool isRunning() const;

		inline void setMessageCallback(MessageCallback callback) {
			recvCallback = callback;
		}

		inline void setConnectCallback(ConnectionCallback callback) {
			connectCallback = callback;
		}

		inline void setDisconnectCallback(ConnectionCallback callback) {
			disconnectCallback = callback;
		}

		template <typename F>
		inline void forEachClient(const F& func);

		inline double getTime() const { return server->GetTime(); }

		template <typename Message_, typename System>
		inline void sendDirectMessageWithSystem(enum GameMessageType messageID,
				enum GameChannelType channel, const System& system,
				const ClientConnection& client);

		template <typename Message_, typename System>
		inline void broadcastMessageWithSystem(enum GameMessageType messageID,
				enum GameChannelType channel, const System& system,
				const ClientConnection* ignore = nullptr);
	private:
		GameAdapter adapter;
		GameConnectionConfig config;
		Memory::UniquePointer<yojimbo::Server> server;

		// TODO: make compile-time configuration for max clients
		// and a more elegant way to deal with all of this
		ClientConnection connections[yojimbo::MaxClients];

		MessageCallback recvCallback;

		ConnectionCallback connectCallback;
		ConnectionCallback disconnectCallback;
};

template <typename F>
inline void NetworkServer::forEachClient(const F& func) {
	for (uint32 i = 0; i < yojimbo::MaxClients; ++i) {
		if (connections[i].isConnected()) {
			func(connections[i]);
		}
	}
}

template <typename Message_, typename System>
inline void NetworkServer::sendDirectMessageWithSystem(
		enum GameMessageType messageID, enum GameChannelType channel,
		const System& system, const ClientConnection& client) {
	Message_* msg = static_cast<Message_*>(server->CreateMessage(
			client.getIndex(), static_cast<int>(messageID)));

	system(msg);

	server->SendMessage(client.getIndex(), static_cast<int>(channel), msg);
}

template <typename Message_, typename System>
inline void NetworkServer::broadcastMessageWithSystem(
		enum GameMessageType messageID, enum GameChannelType channel,
		const System& system, const ClientConnection* ignore) {
	forEachClient([&](const auto& client) {
		if (&client != ignore) {
			Message_* msg = static_cast<Message_*>(server->CreateMessage(
					client.getIndex(), static_cast<int>(messageID)));

			system(msg);

			server->SendMessage(client.getIndex(), static_cast<int>(channel),
					msg);
		}
	});
}

