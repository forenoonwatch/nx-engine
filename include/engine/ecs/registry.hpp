#pragma once

#include <engine/core/common.hpp>
#include <engine/core/singleton.hpp>

#include <engine/ecs/ecs-fwd.hpp>

#include <entt/entity/registry.hpp>

namespace ECS {
	class Registry final : public entt::registry, public Singleton<Registry> {
		public:
		private:
	};
};

