#include "engine/networking/client.hpp"

#include <engine/networking/network-object.hpp>

#include <engine/ecs/registry.hpp>

#include <engine/math/math.hpp>

#include <algorithm>

NetworkClient::NetworkClient()
		: adapter(nullptr)
		, client(yojimbo::GetDefaultAllocator(), yojimbo::Address("0.0.0.0"),
				config, adapter, 0.0) {}

void NetworkClient::connect(const char* serverIP, uint32 serverPort,
		const uint8* privateKey) {
	serverAddress = yojimbo::Address(serverIP, serverPort);
	
	// TODO: better handle client ID
	uint64 clientID = 0;
	yojimbo::random_bytes(reinterpret_cast<uint8*>(&clientID), 8);
	printf("ClientID is %.16" PRIx64 "\n", clientID);

	// TODO: see about secure connection
	client.InsecureConnect(privateKey, clientID, serverAddress);

	// TODO: probably remove/encapsulate this
	char addressString[256];
	client.GetAddress().ToString(addressString, sizeof(addressString));
	printf("Client address: %s\n", addressString);
}

void NetworkClient::receiveMessages() {
	constexpr const double deltaTime = 1.0 / 60.0;
	
	client.AdvanceTime(client.GetTime() + deltaTime);
	client.ReceivePackets();
	
	if (client.IsConnected()) {
		yojimbo::Message* msg;

		for (uint32 i = 0; i < config.numChannels; ++i) {
			while ((msg = client.ReceiveMessage(i)) != nullptr) {
				processMessage(msg);
				client.ReleaseMessage(msg);
			}
		}
	}
}

void NetworkClient::sendMessages() {
	client.SendPackets();
}

void NetworkClient::disconnect() {
	client.Disconnect();
}

bool NetworkClient::isConnected() const {
	return !client.IsDisconnected() && !client.ConnectionFailed();
}

bool NetworkClient::canSendMessage(enum GameChannelType channel) const {
	return client.CanSendMessage(static_cast<int>(channel));
}

void NetworkClient::processMessage(yojimbo::Message* msg) {
	switch (msg->GetType()) {
		default:
			puts("Whatever");
	}
}

