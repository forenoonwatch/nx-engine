#pragma once

#include <engine/game/game.hpp>

class Scene {
	public:
		inline Scene() {}

		virtual void load(Game& game) = 0;
		virtual void unload(Game& game) = 0;

		inline void update(Game& game, float deltaTime);
		inline void render(Game& game, float deltaTime);

		inline void addUpdateSystem(Game::ECSSystemCallback callback);
		inline void addRenderSystem(Game::ECSSystemCallback callback);

		inline void insertUpdateSystem(Game::ECSSystemCallback callback,
				uint32 position);
		inline void insertRenderSystem(Game::ECSSystemCallback callback,
				uint32 position);

		virtual ~Scene() {}
	private:
		NULL_COPY_AND_ASSIGN(Scene);

		ArrayList<Game::ECSSystemCallback> updatePipeline;
		ArrayList<Game::ECSSystemCallback> renderPipeline;
};

inline void Scene::update(Game& game, float deltaTime) {
	for (auto it = std::begin(updatePipeline), end = std::end(updatePipeline);
			it != end; ++it) {
		(*it)(game, deltaTime);
	}
}

inline void Scene::render(Game& game, float deltaTime) {
	for (auto it = std::begin(renderPipeline), end = std::end(renderPipeline);
			it != end; ++it) {
		(*it)(game, deltaTime);
	}
}

inline void Scene::addUpdateSystem(Game::ECSSystemCallback callback) {
	updatePipeline.push_back(callback);
}

inline void Scene::addRenderSystem(Game::ECSSystemCallback callback) {
	renderPipeline.push_back(callback);
}

inline void Scene::insertUpdateSystem(Game::ECSSystemCallback callback,
		uint32 position) {
	updatePipeline.insert(updatePipeline.begin() + position, callback);
}

inline void Scene::insertRenderSystem(Game::ECSSystemCallback callback,
		uint32 position) {
	renderPipeline.insert(renderPipeline.begin() + position, callback);
}

