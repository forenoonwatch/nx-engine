#pragma once

//#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <bullet/BulletDynamics/Dynamics/btActionInterface.h>
#include <bullet/LinearMath/btVector3.h>

//class VehicleBody final : public btGhostObject {};

class btCollisionShape;
class btRigidBody;
class btCollisionWorld;
class btCollisionDispatcher;
class btPairCachingGhostObject;

class KinematicVehicleController : public btActionInterface {
	public:
		KinematicVehicleController(btPairCachingGhostObject* ghostObject,
				btCollisionShape* shape, const btVector3& up = btVector3(0, 1, 0));

		virtual void updateAction(btCollisionWorld* collisionWorld,
				btScalar deltaTime) override;

		virtual void debugDraw(btIDebugDraw* debugDrawer) override;

		void setLinearVelocity(const btVector3& linearVelocity) { this->linearVelocity = linearVelocity; }

		btPairCachingGhostObject* getGhostObject() { return ghostObject; }

		btVector3& getLinearVelocity() { return linearVelocity; }
		const btVector3& getLinearVelocity() const { return linearVelocity; }

		virtual ~KinematicVehicleController();
	private:
		btPairCachingGhostObject* ghostObject;
		btCollisionShape* collisionShape;

		btVector3 linearVelocity;

		btVector3 currentPosition;
		btVector3 targetPosition; // TODO: see if these are necessary

		btVector3 upAxis;
};

class VehicleController {
	public:
		VehicleController(KinematicVehicleController* controller)
				: controller(controller) {}

		KinematicVehicleController* getController() { return controller; }
	private:
		KinematicVehicleController* controller;
};
