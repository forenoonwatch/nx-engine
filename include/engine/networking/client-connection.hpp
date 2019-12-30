#pragma once

#include <engine/networking/state-jitter-buffer.hpp>

#include <engine/core/common.hpp>

class NetworkServer;

class ClientConnection {
	public:
		inline ClientConnection()
				: connected(false) {}

		bool getStateUpdate(StateUpdate& stateUpdate);

		inline bool isConnected() const { return connected; }
	private:
		NULL_COPY_AND_ASSIGN(ClientConnection);

		StateJitterBuffer jitterBuffer;

		uint32 index;
		uint64 clientID;

		bool connected;

		NetworkServer* server;

		inline void markConnected(NetworkServer* server, uint32 index,
				uint64 clientID) {
			this->index = index;
			this->clientID = clientID;
			this->connected = true;
			this->server = server;
		}

		inline void markDisconnected() {
			connected = false;
		}

		friend class NetworkServer;
};
