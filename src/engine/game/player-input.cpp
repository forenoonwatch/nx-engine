#include "engine/game/player-input.hpp"

#include "engine/core/application.hpp"

#include "engine/game/game.hpp"

void PlayerInputSystem::operator()(Game& game, float deltaTime) {
	game.getECS().view<PlayerInputComponent>().each([&](auto& pic) {
		pic.forward = game.getApplication().isKeyDown(Input::KEY_W);
		pic.back = game.getApplication().isKeyDown(Input::KEY_S);
		pic.left = game.getApplication().isKeyDown(Input::KEY_A);
		pic.right = game.getApplication().isKeyDown(Input::KEY_D);
		// TODO: up/down for Q/E

		pic.jump = game.getApplication().isKeyDown(Input::KEY_SPACE);

		pic.leftMouse = game.getApplication().isMouseDown(
				Input::MOUSE_BUTTON_LEFT);
		pic.rightMouse = game.getApplication().isMouseDown(
				Input::MOUSE_BUTTON_RIGHT);

		pic.mouseDeltaX = (float)game.getApplication().getMouseDeltaX();
		pic.mouseDeltaY = (float)game.getApplication().getMouseDeltaY();

		// TODO: scroll wheel delta
	});
}

