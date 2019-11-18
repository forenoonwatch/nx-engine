#pragma once

#include <engine/core/array-list.hpp>
#include <engine/core/memory.hpp>

#include <engine/ecs/ecs-system.hpp>

class Scene {
	public:
		inline Scene() {}

		virtual void load(Game& game) = 0;
		virtual void unload(Game& game) = 0;

		inline void update(Game& game, float deltaTime);
		inline void render(Game& game, float deltaTime);

		inline void addUpdateSystem(ECS::System* callback);
		inline void addRenderSystem(ECS::System* callback);

		inline void insertUpdateSystem(ECS::System* callback, uint32 position);
		inline void insertRenderSystem(ECS::System* callback, uint32 position);

		virtual ~Scene() {}
	private:
		NULL_COPY_AND_ASSIGN(Scene);

		ArrayList<Memory::SharedPointer<ECS::System>> updatePipeline;
		ArrayList<Memory::SharedPointer<ECS::System>> renderPipeline;
};

inline void Scene::update(Game& game, float deltaTime) {
	for (auto& updateCallback : updatePipeline) {
		(*updateCallback)(game, deltaTime);
	}
}

inline void Scene::render(Game& game, float deltaTime) {
	for (auto& renderCallback : renderPipeline) {
		(*renderCallback)(game, deltaTime);
	}
}

inline void Scene::addUpdateSystem(ECS::System* callback) {
	updatePipeline.emplace_back(callback);
}

inline void Scene::addRenderSystem(ECS::System* callback) {
	renderPipeline.emplace_back(callback);
}

inline void Scene::insertUpdateSystem(ECS::System* callback,
		uint32 position) {
	updatePipeline.emplace(updatePipeline.begin() + position, callback);
}

inline void Scene::insertRenderSystem(ECS::System* callback,
		uint32 position) {
	renderPipeline.emplace(renderPipeline.begin() + position, callback);
}

