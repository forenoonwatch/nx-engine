#include "engine/game/player-input.hpp"

#include "engine/ecs/ecs.hpp"
#include "engine/core/application.hpp"

void playerInputSystem(float deltaTime) {
	ECS::Registry::getInstance().view<PlayerInputComponent>().each([&](
			auto& pic) {
		pic.forward = Application::getInstance().isKeyDown(Input::KEY_W);
		pic.back = Application::getInstance().isKeyDown(Input::KEY_S);
		pic.left = Application::getInstance().isKeyDown(Input::KEY_A);
		pic.right = Application::getInstance().isKeyDown(Input::KEY_D);
		// TODO: up/down for Q/E

		pic.jump = Application::getInstance().isKeyDown(Input::KEY_SPACE);

		pic.leftMouse = Application::getInstance().isMouseDown(
				Input::MOUSE_BUTTON_LEFT);
		pic.rightMouse = Application::getInstance().isMouseDown(
				Input::MOUSE_BUTTON_RIGHT);

		pic.mouseDeltaX = (float)Application::getInstance().getMouseDeltaX();
		pic.mouseDeltaY = (float)Application::getInstance().getMouseDeltaY();

		// TODO: scroll wheel delta
	});
}

