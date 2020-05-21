#pragma once

class Registry;
class Application;

struct PlayerInputComponent {
	bool forward = false;
	bool back = false;
	bool left = false;
	bool right = false;

	bool jump = false;

	bool leftMouse = false;
	bool rightMouse = false;

	float mouseDeltaX = 0.f;
	float mouseDeltaY = 0.f;
};

void updatePlayerInput(Registry& registry, Application& app, float deltaTime);
