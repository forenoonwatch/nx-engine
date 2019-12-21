#pragma once

#include <engine/ecs/ecs-system.hpp>

class PhysicsMotionController : public ECS::System {
	public:
		virtual void operator()(Game& game, float deltaTime) override;
	private:
};

