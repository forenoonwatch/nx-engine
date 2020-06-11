#pragma once

#include <engine/core/common.hpp>
#include <engine/core/service.hpp>
#include <engine/core/memory.hpp>

#include <engine/scene/scene.hpp>

class SceneManager final : public Service<SceneManager> {
	public:
		SceneManager();

		template <typename SceneType>
		void load();

		void stop();

		void setFPSUnlocked(bool unlockFPS);

		inline uint32 getFPS() const { return fps; }
		inline bool isFPSUnlocked() const { return unlockFPS; }

		~SceneManager();
	private:
		NULL_COPY_AND_ASSIGN(SceneManager);

		Memory::SharedPointer<Scene> currentScene;

		bool running;

		uint32 fps;
		bool unlockFPS;

		void run();
};

template <typename SceneType>
void SceneManager::load() {
	stop();

	SceneType* newScene = new SceneType(); // TODO: custom allocator
	currentScene = Memory::SharedPointer<Scene>(newScene);

	newScene->load();
	run();
}
