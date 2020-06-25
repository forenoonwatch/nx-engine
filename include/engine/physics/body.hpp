#pragma once

#include <engine/core/common.hpp>

#include <engine/math/vector.hpp>

#include <bullet/BulletDynamics/Dynamics/btRigidBody.h>

class Registry;
class Transform;

class Body {
	public:
		inline Body()
				: handle(nullptr) {}

		void applyImpulse(const Vector3f& impulse);
		void applyImpulse(const Vector3f& impulse, const Vector3f& relPos);

		void setFriction(float friction);

		void setInvInertiaDiagLocal(const Vector3f& invInertia);

		void setCenterOfMassTransform(const Transform& tr);

		float getFriction() const;

		void getCenterOfMassTransform(Transform& tr) const;
		void getRenderTransform(Transform& tr) const;

		bool isAwake() const;
		void setToAwake();

		inline btRigidBody* getHandle() { return handle; }
	private:
		btRigidBody* handle;

		inline Body(btRigidBody* handle)
				: handle(handle) {}

		friend class PhysicsEngine;
};
