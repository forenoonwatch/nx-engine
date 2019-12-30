#pragma once

#include <engine/core/singleton.hpp>

#include <engine/physics/dynamics/body.hpp>
#include <engine/physics/dynamics/contact-manager.hpp>

#include <engine/core/array-list.hpp>

namespace Physics {
	class Collider;

	class PhysicsEngine final : public Singleton<PhysicsEngine> {
		public:
			PhysicsEngine();

			Body* addBody(const BodyHints& hints);

			void step(float deltaTime);

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

