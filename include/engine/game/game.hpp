#pragma once

#include <engine/core/common.hpp>
#include <engine/core/memory.hpp>
#include <engine/core/string.hpp>
#include <engine/core/array-list.hpp>

#include <engine/core/window.hpp>

#include <engine/game/game-render-context.hpp>
#include <engine/game/asset-manager.hpp>

#include <engine/ecs/ecs.hpp>

class Scene;

class Game {
	public:
		typedef void (*ECSSystemCallback)(Game&, float);

		inline Game(Window& window, Memory::SharedPointer<Scene> initialScene,
					bool unlockFPS)
				: window(&window)
				, renderContext(*this)
				, assetManager(renderContext)
				, currentScene(nullptr)
				, running(false)
				, unlockFPS(unlockFPS)
				, fps(0) {
			loadScene(initialScene);
		}

		void loadScene(Memory::SharedPointer<Scene> scene);

		inline GameRenderContext& getRenderContext() { return renderContext; }
		inline ECS::Registry& getECS() { return ecs; }
		inline AssetManager& getAssetManager() { return assetManager; }

		inline uint32 getFPS() const { return fps; }
	private:
		NULL_COPY_AND_ASSIGN(Game);

		Window* window;

		GameRenderContext renderContext;
		ECS::Registry ecs;
		AssetManager assetManager;

		Memory::SharedPointer<Scene> currentScene;

		bool running;
		bool unlockFPS;

		uint32 fps;

		void run();
};

