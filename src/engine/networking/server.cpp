#include "engine/networking/server.hpp"

#include <engine/math/math.hpp>

#include <algorithm>

GameServer::GameServer(const char* address, uint32 port,
			const uint8* privateKey)
		: adapter(this)
		, server(yojimbo::GetDefaultAllocator(), privateKey,
				yojimbo::Address(address, port), config, adapter, 0.0) {
	server.Start(yojimbo::MaxClients);

	char addressString[256];
	server.GetAddress().ToString(addressString, sizeof(addressString));
	printf("Started server on address %s\n", addressString);
}

void GameServer::receiveMessages() {
	constexpr const double deltaTime = 1.0 / 60.0;

	server.AdvanceTime(server.GetTime() + deltaTime);
	server.ReceivePackets();

	yojimbo::Message* msg;

	for (uint32 i = 0; i < server.GetMaxClients(); ++i) {
		if (server.IsClientConnected(i)) {
			for (uint32 j = 0; j < config.numChannels; ++j) {
				uint32 numRecvd = 0;

				while ((msg = server.ReceiveMessage(i, j)) != nullptr) {
					processMessage(i, msg);
					server.ReleaseMessage(i, msg);
					++numRecvd;
				}

				DEBUG_LOG_TEMP("++GOT %d MSGS FROM CLIENT %d", numRecvd, i);
			}
		}
	}
}

void GameServer::sendMessages() {
	// TODO: send state updates to clients
	server.SendPackets();
}

void GameServer::stop() {
	server.Stop();
}

bool GameServer::isRunning() const {
	return server.IsRunning();
}

void GameServer::onClientConnected(uint32 clientIndex) {
	connections[clientIndex].markConnected(this, clientIndex,
			server.GetClientId(clientIndex));
}

void GameServer::onClientDisconnected(uint32 clientIndex) {
	connections[clientIndex].markDisconnected();
}

void GameServer::processMessage(uint32 client, yojimbo::Message* msg) {
	switch (msg->GetType()) {
		case (int)GameMessageType::STATE_UPDATE_MESSAGE:
		{
			StateUpdateMessage* sum = static_cast<StateUpdateMessage*>(msg);
			receive(client, sum);
		}
			break;
		default:
			puts("Got an unknown message!");
	}
}

void GameServer::receive(uint32 client, StateUpdateMessage* msg) {
	msg->stateUpdate.sequence = msg->GetId();
	connections[client].jitterBuffer.addStateUpdate(server.GetTime(),
			msg->GetId(), msg->stateUpdate);
}

