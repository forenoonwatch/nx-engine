#include "engine/physics/vehicle-controller.hpp"

#include <engine/core/common.hpp>

#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <bullet/BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <bullet/BulletCollision/CollisionShapes/btCollisionShape.h>
#include <bullet/LinearMath/btDefaultMotionState.h>

KinematicVehicleController::KinematicVehicleController(btPairCachingGhostObject* ghostObject,
			btCollisionShape* collisionShape, const btVector3& up)
		: ghostObject(ghostObject)
		, collisionShape(collisionShape)
		, linearVelocity(0, 0, 0)
		, currentPosition(0, 0, 0)
		, targetPosition(0, 0, 0)
		, upAxis(up) {
}

void KinematicVehicleController::updateAction(btCollisionWorld* collisionWorld, btScalar deltaTime) {
	currentPosition += linearVelocity * deltaTime;

	btTransform newTrans = ghostObject->getWorldTransform();
	newTrans.setOrigin(currentPosition);
	ghostObject->setWorldTransform(newTrans);

	btVector3 minAabb, maxAabb;
	collisionShape->getAabb(ghostObject->getWorldTransform(), minAabb, maxAabb);
	collisionWorld->getBroadphase()->setAabb(ghostObject->getBroadphaseHandle(),
											 minAabb,
											 maxAabb,
											 collisionWorld->getDispatcher());

	collisionWorld->getDispatcher()->dispatchAllCollisionPairs(ghostObject->getOverlappingPairCache(), collisionWorld->getDispatchInfo(), collisionWorld->getDispatcher());
}

void KinematicVehicleController::debugDraw(btIDebugDraw* debugDrawer) {}

KinematicVehicleController::~KinematicVehicleController() {
}
