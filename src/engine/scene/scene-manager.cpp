#include "engine/scene/scene-manager.hpp"

#include <engine/core/time.hpp>

SceneManager::SceneManager()
		: currentScene(nullptr)
		, running(false)
		, fps(0)
		, unlockFPS(false) {}

void SceneManager::stop() {
	if (currentScene) {
		currentScene->unload();
		currentScene = nullptr;
	}

	if (!running) {
		return;
	}

	running = false;
}

SceneManager::~SceneManager() {
	stop();
}

void SceneManager::run() {
	if (running) {
		return;
	}

	constexpr const float frameTime = 1.0 / 60.0;

	uint32 fpsCounter = 0;

	double lastTime = Time::getTime();
	double fpsTimeCounter = 0.0;
	double updateTimer = 1.0;

	double currentTime, passedTime;
	bool shouldRender;

	running = true;

	for (;;) {
		// TODO: this is an invalid read when a scene is stopped
		if (!running) {
			break;
		}

		currentTime = Time::getTime();
		passedTime = currentTime - lastTime;
		lastTime = currentTime;

		fpsTimeCounter += passedTime;
		updateTimer += passedTime;

		if (fpsTimeCounter >= 1.0) {
			fps = fpsCounter;

			fpsTimeCounter = 0.0;
			fpsCounter = 0;
		}

		shouldRender = unlockFPS;

		while (updateTimer >= frameTime) {
			currentScene->update(frameTime);

			updateTimer -= frameTime;
			shouldRender = true;
		}

		if (shouldRender && currentScene) {
			currentScene->render();
			++fpsCounter;
		}
	}
}
