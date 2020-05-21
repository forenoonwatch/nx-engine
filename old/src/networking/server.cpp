#include "engine/networking/server.hpp"

#include <engine/math/math.hpp>

#include <algorithm>

NetworkServer::NetworkServer()
		: adapter(this)
		, server(nullptr)
		, recvCallback(nullptr)
		, connectCallback(nullptr)
		, disconnectCallback(nullptr) {}

void NetworkServer::start(const char* address, uint32 port,
		const uint8* privateKey) {
	if (server) {
		server.reset();
	}

	server = Memory::make_unique<yojimbo::Server>(
			yojimbo::GetDefaultAllocator(), privateKey,
			yojimbo::Address(address, port), config, adapter, 0.0);

	server->Start(yojimbo::MaxClients);
	
	char addressString[256];
	server->GetAddress().ToString(addressString, sizeof(addressString));
	printf("Started server on address %s\n", addressString);
}

void NetworkServer::receiveMessages() {
	constexpr const double deltaTime = 1.0 / 60.0;

	server->AdvanceTime(server->GetTime() + deltaTime);
	server->ReceivePackets();

	yojimbo::Message* msg;

	for (uint32 i = 0; i < server->GetMaxClients(); ++i) {
		if (server->IsClientConnected(i)) {
			for (uint32 j = 0; j < config.numChannels; ++j) {
				while ((msg = server->ReceiveMessage(i, j)) != nullptr) {
					recvCallback(connections[i], msg);
					server->ReleaseMessage(i, msg);
				}
			}
		}
	}
}

void NetworkServer::sendMessages() {
	// TODO: send state updates to clients
	server->SendPackets();
}

void NetworkServer::stop() {
	server->Stop();
}

bool NetworkServer::isRunning() const {
	return server->IsRunning();
}

void NetworkServer::onClientConnected(uint32 clientIndex) {
	connections[clientIndex].markConnected(this, clientIndex,
			server->GetClientId(clientIndex));

	if (connectCallback) {
		connectCallback(connections[clientIndex]);
	}
}

void NetworkServer::onClientDisconnected(uint32 clientIndex) {
	if (disconnectCallback) {
		disconnectCallback(connections[clientIndex]);
	}

	connections[clientIndex].markDisconnected();
}

