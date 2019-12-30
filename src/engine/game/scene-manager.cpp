#include "engine/game/scene-manager.hpp"

SceneManager::SceneManager()
		: currentScene(nullptr) {}

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

