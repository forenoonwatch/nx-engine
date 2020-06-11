#include "engine/scene/scene-manager.hpp"

SceneManager::SceneManager()
		: currentScene(nullptr)
		, fps(0)
		, unlockFPS(false) {}

void SceneManager::stop() {
	if (currentScene) {
		currentScene->virtualUnload();
		delete currentScene; // TODO: custom alloactor

		currentScene = nullptr;
	}
}

SceneManager::~SceneManager() {
	stop();
}
