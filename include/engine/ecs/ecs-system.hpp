#pragma once

#include <engine/core/common.hpp>

class Game;

namespace ECS {
	class System {
		public:
			virtual void operator()(Game&, float) = 0;

			virtual ~System() {}
		private:
	};
};

