#pragma once

#include <engine/math/vector.hpp>

class Registry;
class RenderSystem;
class Application;

struct CameraComponent {
	Vector3f position;
	float rotationX;
	float rotationY;
	Vector3f rayDirection;
};

void updateCameraComponents(Registry& registry, Application& app, RenderSystem& renderer);
