#include "engine/components/player-input.hpp"

#include <engine/ecs/ecs.hpp>
#include <engine/application/application.hpp>

void updatePlayerInput(Registry& registry, Application& app, float deltaTime) {
	registry.view<PlayerInputComponent>().each([&](
			auto& pic) {
		pic.forward = app.isKeyDown(Input::KEY_W);
		pic.back = app.isKeyDown(Input::KEY_S);
		pic.left = app.isKeyDown(Input::KEY_A);
		pic.right = app.isKeyDown(Input::KEY_D);
		// TODO: up/down for Q/E

		pic.jump = app.getKeyPressed(Input::KEY_SPACE);

		pic.leftMouse = app.isMouseDown(Input::MOUSE_BUTTON_LEFT);
		pic.rightMouse = app.isMouseDown(Input::MOUSE_BUTTON_RIGHT);

		pic.mouseDeltaX = static_cast<float>(app.getMouseDeltaX());
		pic.mouseDeltaY = static_cast<float>(app.getMouseDeltaY());

		// TODO: scroll wheel delta
	});
}
