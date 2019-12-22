#include "engine/networking/client.hpp"

#include <engine/math/math.hpp>

#include <algorithm>

GameClient::GameClient(const char* serverIP, uint32_t serverPort,
			const uint8_t* privateKey)
		: adapter(nullptr)
		, serverAddress(serverIP, serverPort)
		, client(yojimbo::GetDefaultAllocator(), yojimbo::Address("0.0.0.0"),
				config, adapter, 0.0) {
	uint64_t clientID = 0;
	yojimbo::random_bytes(reinterpret_cast<uint8_t*>(&clientID), 8);
	printf("ClientID is %.16" PRIx64 "\n", clientID);

	client.InsecureConnect(privateKey, clientID, serverAddress);

	char addressString[256];
	client.GetAddress().ToString(addressString, sizeof(addressString));
	printf("Client address: %s\n", addressString);
}

void GameClient::receiveMessages() {
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

void GameClient::sendMessages() {
	if (client.IsConnected()) {
		accumulatePriorities();

		if (client.CanSendMessage((int)GameChannelType::UNRELIABLE)) {
			StateUpdateMessage* sum
					= (StateUpdateMessage*)client.CreateMessage(
					(int)GameMessageType::STATE_UPDATE_MESSAGE);

			sum->stateUpdate.inputState = inputState;
			sum->stateUpdate.numBodies = Math::min(
					(size_t)StateUpdate::MAX_BODIES,
					priorityBuffer.size());

			for (uint32 i = 0; i < sum->stateUpdate.numBodies; ++i) {
				sum->stateUpdate.networkIDs[i]
						= priorityBuffer.front().networkID;
				// TODO: set contents of bodyStates[i]

				std::pop_heap(std::begin(priorityBuffer),
						std::end(priorityBuffer));
				priorityBuffer.back().accumulatedPriority = 0;
			}

			client.SendMessage((int)GameChannelType::UNRELIABLE, sum);
		}

		client.SendPackets();
	}
}

bool GameClient::isConnected() const {
	return !client.IsDisconnected() && !client.ConnectionFailed();
}

void GameClient::stop() {
	client.Disconnect();
}

void GameClient::processMessage(yojimbo::Message* msg) {
	switch (msg->GetType()) {
		default:
			puts("Whatever");
	}
}

void GameClient::addInputState(const InputState& is) {
	this->inputState = is;
}

void GameClient::addNetworkObject(const NetworkObject& netObj) {
	priorityBuffer.push_back(netObj);
	std::push_heap(std::begin(priorityBuffer), std::end(priorityBuffer));
}

void GameClient::accumulatePriorities() {
	for (auto& n : priorityBuffer) {
		n.accumulatedPriority += n.networkPriority;
	}

	std::make_heap(std::begin(priorityBuffer), std::end(priorityBuffer));
}

