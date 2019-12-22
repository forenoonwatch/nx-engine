#include "engine/networking/net-adapter.hpp"

#include "engine/networking/server.hpp"

yojimbo::MessageFactory* GameAdapter::CreateMessageFactory(
		yojimbo::Allocator& allocator) {
	return YOJIMBO_NEW(allocator, GameMessageFactory, allocator);
}

void GameAdapter::OnServerClientConnected(int clientIndex) {
	if (server != nullptr) {
		server->onClientConnected(clientIndex);
	}
}

void GameAdapter::OnServerClientDisconnected(int clientIndex) {
	if (server != nullptr) {
		server->onClientDisconnected(clientIndex);
	}
}

