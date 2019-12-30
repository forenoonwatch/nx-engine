#include "engine/game/game-event-handler.hpp"

#include "engine/core/window.hpp"

#include "engine/game/scene-manager.hpp"
#include "engine/game/input-control.hpp"

#include <algorithm>

void GameEventHandler::onKeyDown(enum Input::KeyCode keyCode) {
	updateInput((uint32)keyCode, 1.f);
}

void GameEventHandler::onKeyUp(enum Input::KeyCode keyCode) {
	updateInput((uint32)keyCode, -1.f);
}

void GameEventHandler::onMouseDown(enum Input::MouseButton mouseButton) {
	updateInput(MOUSE_OFFSET + (uint32)mouseButton, 1.f);
}

void GameEventHandler::onMouseUp(enum Input::MouseButton mouseButton) {
	updateInput(MOUSE_OFFSET + (uint32)mouseButton, -1.f);
}

void GameEventHandler::onWindowClosed(Window& window) {
	SceneManager::getInstance().stop();
}

void GameEventHandler::addKeyControl(enum Input::KeyCode keyCode,
		InputControl& inputControl, float weight) {
	inputs[(uint32)keyCode].emplace_back(weight, inputControl);
}

void GameEventHandler::updateInput(uint32 inputCode, float dir) {
	std::for_each(std::begin(inputs[inputCode]), std::end(inputs[inputCode]),
			[inputCode, dir](auto& ic) {
		ic.second.add(dir * ic.first);
	});
}

