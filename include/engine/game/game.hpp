#pragma once

#include <engine/core/common.hpp>
#include <engine/core/memory.hpp>
#include <engine/core/string.hpp>
#include <engine/core/array-list.hpp>

#include <engine/core/window.hpp>

#include <engine/game/asset-manager.hpp>

#include <engine/rendering/render-context.hpp>

#include <engine/ecs/ecs.hpp>

#include <functional>

class Scene;

class Game {
	public:
		typedef std::function<void(Game&, float)> ECSSystemCallback;

		inline Game(Window& window, RenderContext* renderContext,
					bool unlockFPS)
				: window(&window)
				, renderContext(renderContext)
				, assetManager(renderContext)
				, currentScene(nullptr)
				, running(false)
				, unlockFPS(unlockFPS)
				, fps(0) {
			renderContext->setGame(*this);
		}

		void loadScene(Memory::SharedPointer<Scene> scene);

		inline RenderContext* getRenderContext() { return renderContext; }
		
		inline ECS::Registry& getECS() { return ecs; }
		inline AssetManager& getAssetManager() { return assetManager; }

		inline uint32 getFPS() const { return fps; }
	private:
		NULL_COPY_AND_ASSIGN(Game);

		Window* window;

		RenderContext* renderContext;
		
		ECS::Registry ecs;
		AssetManager assetManager;

		Memory::SharedPointer<Scene> currentScene;

		bool running;
		bool unlockFPS;

		uint32 fps;

		void run();
};

