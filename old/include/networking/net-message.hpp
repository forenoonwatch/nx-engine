#pragma once

#include <yojimbo/yojimbo.h>

#include <engine/networking/state-update-message.hpp>
#include <engine/networking/player-connect-message.hpp>

enum GameMessageType {
	STATE_UPDATE_MESSAGE = 0,
	PLAYER_CONNECT_MESSAGE = 1,

	NUM_GAME_MESSAGE_TYPES
};

YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, NUM_GAME_MESSAGE_TYPES);

YOJIMBO_DECLARE_MESSAGE_TYPE(STATE_UPDATE_MESSAGE, StateUpdateMessage);
YOJIMBO_DECLARE_MESSAGE_TYPE(PLAYER_CONNECT_MESSAGE, PlayerConnectMessage);

YOJIMBO_MESSAGE_FACTORY_FINISH();

