#pragma once

#include <yojimbo/yojimbo.h>

class NetworkServer;

class GameAdapter : public yojimbo::Adapter {
	public:
		inline explicit GameAdapter(NetworkServer* server)
				: server(server) {}

		virtual yojimbo::MessageFactory* CreateMessageFactory(
				yojimbo::Allocator& allocator) override;

		virtual void OnServerClientConnected(int clientIndex) override;
		virtual void OnServerClientDisconnected(int clientIndex) override;
	private:
		NetworkServer* server;
};

