#pragma once

#include <engine/core/common.hpp>

#include <engine/math/vector.hpp>

#include <bullet/BulletDynamics/Dynamics/btRigidBody.h>

class Registry;
class Transform;

class Body {
	public:
		void applyImpulse(const Vector3f& impulse);
		void applyImpulse(const Vector3f& impulse, const Vector3f& relPos);

		void setInvInertiaDiagLocal(const Vector3f& invInertia);

		void setCenterOfMassTransform(const Transform& tr);

		void getCenterOfMassTransform(Transform& tr);
		void getRenderTransform(Transform& tr);

		bool isAwake();
		void setToAwake();

		inline btRigidBody* getHandle() { return handle; }
	private:
		btRigidBody* handle;

		inline Body(btRigidBody* handle)
				: handle(handle) {}

		friend class PhysicsEngine;
};
