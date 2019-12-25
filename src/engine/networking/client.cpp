#include "engine/networking/client.hpp"

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

bool NetworkClient::isConnected() const {
	return !client.IsDisconnected() && !client.ConnectionFailed();
}

void NetworkClient::disconnect() {
	client.Disconnect();
}

void NetworkClient::processMessage(yojimbo::Message* msg) {
	switch (msg->GetType()) {
		default:
			puts("Whatever");
	}
}

void NetworkClient::addInputState(const InputState& is) {
	this->inputState = is;
}

void NetworkClient::addNetworkObject(const NetworkObject& netObj) {
	priorityBuffer.push_back(netObj);
	std::push_heap(std::begin(priorityBuffer), std::end(priorityBuffer));
}

void NetworkClient::accumulatePriorities() {
	for (auto& n : priorityBuffer) {
		n.accumulatedPriority += n.networkPriority;
	}

	std::make_heap(std::begin(priorityBuffer), std::end(priorityBuffer));
}

