#pragma once

#include <engine/core/common.hpp>
#include <engine/core/singleton.hpp>

#include <engine/scene/scene.hpp>

class SceneManager final : public Singleton<SceneManager> {
	public:
		SceneManager();

		template <typename SceneType>
		void load();

		void stop();

		~SceneManager();
	private:
		NULL_COPY_AND_ASSIGN(SceneManager);

		BaseScene* currentScene;
};

template <typename SceneType>
void SceneManager::load() {
	stop();

	SceneType* newScene = new SceneType(); // TODO: custom allocator
	currentScene = static_cast<BaseScene*>(newScene);

	newScene->load();
	newScene->run();
}
