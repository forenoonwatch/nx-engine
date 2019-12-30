#include "engine/game/physics-motion-controller.hpp"

#include "engine/game/player-input.hpp"
#include "engine/game/camera.hpp"

#include "engine/ecs/ecs.hpp"

#include "engine/physics/dynamics/body.hpp"

#include <algorithm>

#define PLAYER_SPEED_TEMP 0.1f

void physicsMotionController(float deltaTime) {
	ECS::Registry::getInstance().view<Physics::BodyHandle,
			PlayerInputComponent, CameraComponent>().each([&](auto& handle,
			auto& pic, auto& cc) {
		Vector3f v(0.f);

		if (pic.forward) {
			v.z -= 1.f;
		}

		if (pic.back) {
			v.z += 1.f;
		}

		if (pic.left) {
			v.x -= 1.f;
		}

		if (pic.right) {
			v.x += 1.f;
		}

		v = Vector3f(cc.camera->view * Vector4f(v, 0.f));
		v.y = 0.f;
		v = Math::dot(v, v) < 1e-5f ? v : Math::normalize(v)
				* PLAYER_SPEED_TEMP;

		if (pic.jump) {
			v.y += 0.5f;
		}

		handle.body->getVelocity() += v;
		handle.body->setToAwake();
	});
}

