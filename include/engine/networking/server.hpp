#pragma once

#include <engine/networking/net-common.hpp>
#include <engine/networking/client-connection.hpp>

#include <engine/core/common.hpp>
#include <engine/core/memory.hpp>
#include <engine/core/array-list.hpp>

class NetworkServer {
	public:
		NetworkServer();

		void start(const char* address, uint32 port, const uint8* privateKey);

		void receiveMessages();
		void sendMessages();

		void stop();

		void onClientConnected(uint32 clientIndex);
		void onClientDisconnected(uint32 clientIndex);

		bool isRunning() const;

		template <typename F>
		inline void forEachClient(const F& func);

		inline double getTime() const { return server->GetTime(); }
	private:
		GameAdapter adapter;
		GameConnectionConfig config;
		Memory::UniquePointer<yojimbo::Server> server;

		// TODO: make compile-time configuration for max clients
		// and a more elegant way to deal with all of this
		ClientConnection connections[yojimbo::MaxClients];

		void processMessage(uint32, yojimbo::Message*);

		void receive(uint32, StateUpdateMessage*);
};

template <typename F>
inline void NetworkServer::forEachClient(const F& func) {
	for (uint32 i = 0; i < yojimbo::MaxClients; ++i) {
		if (connections[i].isConnected()) {
			func(connections[i]);
		}
	}
}

