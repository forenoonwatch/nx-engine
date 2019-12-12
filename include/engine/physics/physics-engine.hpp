#pragma once

#include <engine/physics/dynamics/body.hpp>
#include <engine/physics/dynamics/contact-manager.hpp>

#include <engine/core/array-list.hpp>

#include <engine/ecs/ecs-system.hpp>

class Game;

namespace Physics {
	class Collider;

	class PhysicsEngine : public ECS::System {
		public:
			PhysicsEngine();

			Body* addBody(const BodyHints& hints);

			virtual void operator()(Game& game, float deltaTime) override;

			void addCollider(Body& body, Collider& collider);

			inline ContactManager& getContactManager();

			inline ArrayList<Body*>& getBodies();

			~PhysicsEngine();
		private:
			ContactManager contactManager;
			ArrayList<Body*> bodies;

			bool newCollider;
	};
};

#include "physics-engine.inl"

