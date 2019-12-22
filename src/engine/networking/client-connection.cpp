#include "engine/networking/client-connection.hpp"

#include "engine/networking/server.hpp"

bool ClientConnection::getStateUpdate(StateUpdate& stateUpdate) {
	return jitterBuffer.getStateUpdate(server->getTime(), stateUpdate);
}

