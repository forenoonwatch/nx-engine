#include "engine/game/util-systems.hpp"

#include "engine/game/camera.hpp"
#include "engine/game/renderable-mesh.hpp"

#include "engine/game/util-components.hpp"

#include <engine/ecs/ecs.hpp>

#include <engine/core/application.hpp>

#include <engine/game/game-render-context.hpp>

void renderMesh() {
	ECS::Registry::getInstance().view<TransformComponent,
			RenderableMesh>().each([&](TransformComponent& transform,
			RenderableMesh& mesh) {
		if (mesh.render) {
			Application::getInstance().renderMesh(*mesh.vertexArray,
					*mesh.material, transform.transform.toMatrix());
		}
	});
}

void renderSkybox() {
	Matrix4f mvp = Math::translate(
			Application::getInstance().getCamera().viewProjection,
			Vector3f(Application::getInstance().getCamera().view[3]));

	Application::getInstance().getSkyboxCube().updateBuffer(1, &mvp,
			sizeof(Matrix4f));
	Application::getInstance().renderSkybox(
			Application::getInstance().getSpecularIBL(),
			Application::getInstance().getLinearMipmapSampler());
}

void toggleFullscreenSystem(float deltaTime) {
	if (Application::getInstance().getKeyPressed(Input::KEY_M)) {
		if (Application::getInstance().isFullscreen()) {
			Application::getInstance().setFullscreen(false);
			Application::getInstance().resizeWindow(1200, 800);
			Application::getInstance().moveToCenter();
		}
		else {
			Application::getInstance().setFullscreen(true);
		}
	}
	else if (Application::getInstance().getKeyPressed(Input::KEY_ESCAPE)
			&& Application::getInstance().isFullscreen()) {
		Application::getInstance().setFullscreen(false);
		Application::getInstance().resizeWindow(1200, 800);
		Application::getInstance().moveToCenter();
	}
}

