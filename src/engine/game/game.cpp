#include "engine/game/game.hpp"

#include "engine/game/scene.hpp"

#include "engine/core/application.hpp"
#include "engine/core/time.hpp"

Game::Game(Application& application, Window& window,
			RenderContext* renderContext, bool unlockFPS)
		: application(&application)
		, window(&window)
		, renderContext(renderContext)
		, assetManager(renderContext)
		, currentScene(nullptr)
		, running(false)
		, unlockFPS(unlockFPS)
		, fps(0) {
	application.addEventHandler(eventHandler);
}

void Game::loadScene(Memory::SharedPointer<Scene> scene) {
	if (currentScene) {
		currentScene->unload(*this);
	}

	currentScene = scene;

	scene->load(*this);

	if (!running) {
		run();
	}
}

void Game::run() {
	const float frameTime = 1.0 / 60.0;

	uint32 fpsCounter = 0;

	double lastTime = Time::getTime();
	double fpsTimeCounter = 0.0;
	double updateTimer = 1.0;

	double currentTime, passedTime;
	bool shouldRender;

	running = true;

	while (!window->isCloseRequested()) {
		currentTime = Time::getTime();
		passedTime = currentTime - lastTime;
		lastTime = currentTime;

		fpsTimeCounter += passedTime;
		updateTimer += passedTime;

		if (fpsTimeCounter >= 1.0) {
			fps = fpsCounter;

			fpsTimeCounter = 0;
			fpsCounter = 0;
		}
		
		shouldRender = unlockFPS;
		
		while (updateTimer >= frameTime) {
			application->pollEvents();
			
			currentScene->update(*this, frameTime);

			updateTimer -= frameTime;
			shouldRender = true;
		}
		
		if (shouldRender) {
			currentScene->render(*this, frameTime);
			
			window->render();
			
			++fpsCounter;
		}
	}

	if (currentScene) {
		currentScene->unload(*this);
	}

	currentScene = nullptr;

	running = false;
}
