#include "engine/components/camera-component.hpp"

#include <engine/application/application.hpp>
#include <engine/ecs/ecs.hpp>

#include <engine/rendering/render-system.hpp>
#include <engine/rendering/camera.hpp>

void updateCameraComponents(Registry& registry, Application& app, RenderSystem& renderer) {
    auto& camera = renderer.getCamera();

    registry.view<CameraComponent>().each([&](auto& cc) {
		// mouse ray
		const float ndcX = (2.f * app.getMouseX()) / static_cast<float>(app.getWidth()) - 1.f;
		const float ndcY = (2.f * app.getMouseY()) / static_cast<float>(app.getHeight()) - 1.f;

		Vector4f rawRay = camera.iProjection * Vector4f(ndcX, -ndcY, -1.f, 1.f);
		rawRay = camera.view * Vector4f(rawRay.x, rawRay.y, -1.f, 0.f);
	
		cc.rayDirection = Math::normalize(Vector3f(rawRay));
    });
}
