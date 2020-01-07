#pragma once

#include <engine/networking/state-jitter-buffer.hpp>

#include <engine/core/common.hpp>

class NetworkServer;

class ClientConnection {
	public:
		inline ClientConnection()
				: connected(false) {}

		inline void addStateUpdate(double time, uint16 sequence,
				const StateUpdate& stateUpdate) {
			jitterBuffer.addStateUpdate(time, sequence, stateUpdate);
		}	

		bool getStateUpdate(StateUpdate& stateUpdate);

		inline uint32 getIndex() const { return index; }
		inline uint64 getClientID() const { return clientID; }

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

