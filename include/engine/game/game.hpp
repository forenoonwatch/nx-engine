#pragma once

#include <engine/core/common.hpp>
#include <engine/core/memory.hpp>
#include <engine/core/string.hpp>
#include <engine/core/array-list.hpp>

#include <engine/core/window.hpp>

#include <engine/game/asset-manager.hpp>
#include <engine/game/game-event-handler.hpp>

#include <engine/rendering/render-context.hpp>

#include <engine/ecs/ecs.hpp>

class Scene;

class Game {
	public:
		Game(Application& application, Window& window,
				RenderContext* renderContext, bool unlockFPS);

		void loadScene(Memory::SharedPointer<Scene> scene);

		inline Application& getApplication() { return *application; }
		inline Window& getWindow() { return *window; }

		inline RenderContext* getRenderContext() { return renderContext; }
		
		inline ECS::Registry& getECS() { return ecs; }
		inline AssetManager& getAssetManager() { return assetManager; }
		inline GameEventHandler& getEventHandler() { return eventHandler; }

		inline uint32 getFPS() const { return fps; }
	private:
		NULL_COPY_AND_ASSIGN(Game);

		Application* application;
		Window* window;

		RenderContext* renderContext;
		
		ECS::Registry ecs;
		AssetManager assetManager;
		GameEventHandler eventHandler;

		Memory::SharedPointer<Scene> currentScene;

		bool running;
		bool unlockFPS;

		uint32 fps;

		void run();
};

