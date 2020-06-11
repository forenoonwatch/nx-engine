#pragma once

class Scene {
	public:
		Scene() = default;

		virtual void load() = 0;

		virtual void update(float deltaTime) = 0;
		virtual void render() = 0;

		virtual void unload() = 0;

		virtual ~Scene() = default;
	private:
		friend class SceneManager;
};
