#include "engine/physics/body.hpp"

#include <engine/physics/physics-util.hpp>

void Body::applyImpulse(const Vector3f& impulse) {
	handle->applyCentralImpulse(Physics::nativeToBtVec3(impulse));
}

void Body::applyImpulse(const Vector3f& impulse, const Vector3f& relPos) {
	handle->applyImpulse(Physics::nativeToBtVec3(impulse),
			Physics::nativeToBtVec3(relPos));
}

void Body::setInvInertiaDiagLocal(const Vector3f& invInertia) {
	handle->setInvInertiaDiagLocal(Physics::nativeToBtVec3(invInertia));
}

void Body::setCenterOfMassTransform(const Transform& tr) {
	btTransform btTr;
	Physics::nativeToBtTransform(btTr, tr);

	handle->setCenterOfMassTransform(btTr);
}

void Body::getCenterOfMassTransform(Transform& tr) {
	Physics::btToNativeTransform(tr, handle->getCenterOfMassTransform());
}

void Body::getRenderTransform(Transform& tr) {
	btTransform btTr;
	handle->getMotionState()->getWorldTransform(btTr);

	Physics::btToNativeTransform(tr, btTr);
}

bool Body::isAwake() {
	return handle->isActive();
}

void Body::setToAwake() {
	handle->activate();
}
