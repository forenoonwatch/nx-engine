#include "engine/game/camera.hpp"

#include "engine/game/util-components.hpp"

#include <engine/ecs/ecs.hpp>
#include <engine/core/application.hpp>

void updateCameraSystem(float deltaTime) {
	ECS::Registry::getInstance().view<TransformComponent,
			CameraComponent>().each([&](TransformComponent& transform,
			CameraComponent& cc) {
		Camera& camera = *cc.camera;

		camera.iView = Math::inverse(camera.view);

		const Matrix4f iProjection = Math::inverse(camera.projection);
		
		camera.viewProjection = camera.projection * camera.iView;
		camera.iViewProjection = camera.view * iProjection;

		// mouse ray
		const float ndcX = (2.f * Application::getInstance().getMouseX())
				/ (float)Application::getInstance().getWidth()
				- 1.f;
		const float ndcY = (2.f * Application::getInstance().getMouseY())
				/ (float)Application::getInstance().getHeight()
				- 1.f;

		Vector4f rawRay = iProjection
				* Vector4f(ndcX, -ndcY, -1.f, 1.f);
		rawRay = camera.view * Vector4f(rawRay.x, rawRay.y, -1.f, 0.f);
	
		cc.rayDirection = Math::normalize(Vector3f(rawRay));
	});
}

