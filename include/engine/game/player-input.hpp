#pragma once

#include <engine/core/common.hpp>

#include <engine/ecs/ecs-system.hpp>

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

class PlayerInputSystem final : public ECS::System {
	public:
		virtual void operator()(Game& game, float deltaTime) override;
};

