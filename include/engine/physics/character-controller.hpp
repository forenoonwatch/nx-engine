#pragma once

#include <engine/physics/kinematic-character-controller.hpp>

#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>

class Collider;

class CharacterController {
	public:
		inline CharacterController()
				: ghostObject(nullptr)
				, controller(nullptr) {}
		
		inline CharacterController(btPairCachingGhostObject* ghostObject,
					KinematicCharacterController* controller)
				: ghostObject(ghostObject)
				, controller(controller) {}

		inline btPairCachingGhostObject* getGhostObject() { return ghostObject; }
		inline KinematicCharacterController* getController() { return controller; }
	private:
		btPairCachingGhostObject* ghostObject;
		KinematicCharacterController* controller;
};
