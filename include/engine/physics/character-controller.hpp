#pragma once

#include <engine/physics/kinematic-character-controller.hpp>

#include <bullet/LinearMath/btVector3.h>

#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <bullet/BulletDynamics/Character/btCharacterControllerInterface.h>

class btConvexShape;
class btCollisionWorld;
class btPairCachingGhostObject;

class CharacterController final : public btActionInterface {
	public:
		BT_DECLARE_ALIGNED_ALLOCATOR();

		CharacterController(btPairCachingGhostObject* ghostObject,
				btConvexShape* convexShape);

		virtual void updateAction(btCollisionWorld* collisionWorld,
				btScalar deltaTime) override;

		virtual void debugDraw(btIDebugDraw* debugDrawer) override;

		void setWalkDirection(const btVector3& walkDirection);

		btPairCachingGhostObject* getGhostObject();

		const btVector3& getWalkDirection() const;

		virtual ~CharacterController();
	private:
		btPairCachingGhostObject* ghostObject;
		btConvexShape* convexShape;

		btScalar halfHeight;

		btVector3 walkDirection;
};
