#pragma once

#include <engine/core/common.hpp>
#include <engine/core/time.hpp>

class BaseScene {
	public:
		virtual void virtualUnload() = 0;

		virtual ~BaseScene() = default;
};

template <typename DerivedScene>
class Scene : public BaseScene {
	public:
		Scene();

		void load();

		void update(float deltaTime);
		void render();

		void unload();

		virtual void virtualUnload() override final;
	private:
		bool running;

		void run();
		void stop();

		friend class SceneManager;
};

#include "engine/game/scene.inl"

