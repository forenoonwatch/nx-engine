#include "engine/physics/dynamics/contact-constraint.hpp"

#include <engine/physics/dynamics/body.hpp>
#include <engine/physics/collision/collision.hpp>

#include <engine/math/math.hpp>

#define BAUMGARTE			0.2f
#define PENETRATION_SLOP	0.05f

Physics::ContactConstraint::ContactConstraint(Collider& a, Collider& b)
		: colliderA(&a)
		, colliderB(&b)
		, bodyA(a.getBody())
		, bodyB(b.getBody())
		, edgeA(Memory::make_shared<ContactEdge>())
		, edgeB(Memory::make_shared<ContactEdge>())
		, friction(mixFriction())
		, restitution(mixRestitution())
		, flags(0) {
	manifold.setPair(a, b);

	edgeA->constraint = this;
	edgeA->other = b.getBody();

	edgeB->constraint = this;
	edgeB->other = a.getBody();

	a.getBody()->contactList.push_back(edgeA.get());
	b.getBody()->contactList.push_back(edgeB.get());
}

void Physics::ContactConstraint::testCollision() {
	manifold.numContacts = 0;
	CollisionCallback cb = COLLISION_DISPATCH[colliderA->getType()]
			[colliderB->getType()];

	if (cb != nullptr) {
		cb(manifold, *(colliderA), *(colliderB));
	}

	if (manifold.numContacts > 0) {
		if (flags & FLAG_COLLIDING) {
			flags |= FLAG_WAS_COLLIDING;
		}
		else {
			flags |= FLAG_COLLIDING;
		}

		Math::computeBasis(manifold.normal, manifold.tangents[0],
				manifold.tangents[1]);
	}
	else {
		if (flags & FLAG_COLLIDING) {
			flags &= ~FLAG_COLLIDING;
			flags |= FLAG_WAS_COLLIDING;
		}
		else {
			flags &= ~FLAG_WAS_COLLIDING;
		}
	}
}

void Physics::ContactConstraint::preSolve(float deltaTime) {
	for (uint32 i = 0; i < manifold.numContacts; ++i) {
		Contact& cs = manifold.contacts[i];

		Vector3f dv = bodyB->velocity
				+ Math::cross(bodyB->angularVelocity, cs.rB) - bodyA->velocity
				- Math::cross(bodyA->angularVelocity, cs.rA);

		// calculate normal and tangent mass
		cs.normalMass = bodyA->invMass + bodyB->invMass;
		cs.tangentMass[0] = cs.tangentMass[1] = cs.normalMass;

		Vector3f rAcN = Math::cross(cs.rA, manifold.normal);
		Vector3f rBcN = Math::cross(cs.rB, manifold.normal);

		cs.normalMass += Math::dot(rAcN, bodyA->invInertiaWorld * rAcN)
				+ Math::dot(rBcN, bodyB->invInertiaWorld * rBcN);
		cs.normalMass = cs.normalMass == 0.f ? 0.f : 1.f / cs.normalMass;

		for (uint32 j = 0; j < 2; ++j) {
			Vector3f rAcT = Math::cross(cs.rA, manifold.tangents[j]);
			Vector3f rBcT = Math::cross(cs.rB, manifold.tangents[j]);

			cs.tangentMass[j] += Math::dot(rAcT, bodyA->invInertiaWorld * rAcT)
				+ Math::dot(rBcT, bodyB->invInertiaWorld * rBcT);
			cs.tangentMass[j] = cs.tangentMass[j] == 0.f ? 0.f
					: 1.f / cs.tangentMass[j];
		}

		// calculate bias
		cs.bias = -BAUMGARTE / deltaTime
				* Math::min(0.f, cs.penetration + PENETRATION_SLOP);
		
		// warm start
		Vector3f p = manifold.normal * cs.normalImpulse;
		p += manifold.tangents[0] * cs.tangentImpulse[0];
		p += manifold.tangents[1] * cs.tangentImpulse[1];

		bodyA->velocity -= p * bodyA->invMass;
		bodyA->angularVelocity -= bodyA->invInertiaWorld
				* Math::cross(cs.rA, p);
		
		bodyB->velocity += p * bodyB->invMass;
		bodyB->angularVelocity += bodyB->invInertiaWorld
				* Math::cross(cs.rB, p);

		// add restitution bias
		float rv = Math::dot(dv, manifold.normal);

		if (rv < -1.f) {
			cs.bias += -restitution * rv;
		}
	}
}

void Physics::ContactConstraint::solve() {
	for (uint32 i = 0; i < manifold.numContacts; ++i) {
		Contact& cs = manifold.contacts[i];

		Vector3f dv = bodyB->velocity
				+ Math::cross(bodyB->angularVelocity, cs.rB) - bodyA->velocity
				- Math::cross(bodyA->angularVelocity, cs.rA);

		for (uint32 j = 0; j < 2; ++j) {
			float lambda = -Math::dot(dv, manifold.tangents[j])
					* cs.tangentMass[j];

			float oldTI = cs.tangentImpulse[j];
			cs.tangentImpulse[j] = Math::clamp(oldTI + lambda,
					-cs.normalImpulse * friction, cs.normalImpulse * friction);
			lambda = cs.tangentImpulse[j] - oldTI;

			Vector3f impulse = manifold.tangents[j] * lambda;

			bodyA->velocity -= impulse * bodyA->invMass;
			bodyA->angularVelocity -= bodyA->invInertiaWorld
					* Math::cross(cs.rA, impulse);

			bodyB->velocity += impulse * bodyB->invMass;
			bodyB->angularVelocity += bodyB->invInertiaWorld
					* Math::cross(cs.rB, impulse);
		}

		dv = bodyB->velocity + Math::cross(bodyB->angularVelocity, cs.rB)
				- bodyA->velocity - Math::cross(bodyA->angularVelocity, cs.rA);

		float rv = Math::dot(dv, manifold.normal);

		float lambda = cs.normalMass * (-rv + cs.bias);

		float oldNI = cs.normalImpulse;
		cs.normalImpulse = Math::max(oldNI + lambda, 0.f);
		lambda = cs.normalImpulse - oldNI;

		Vector3f impulse = manifold.normal * lambda;

		bodyA->velocity -= impulse * bodyA->invMass;
		bodyA->angularVelocity -= bodyA->invInertiaWorld
				* Math::cross(cs.rA, impulse);

		bodyB->velocity += impulse * bodyB->invMass;
		bodyB->angularVelocity += bodyB->invInertiaWorld
				* Math::cross(cs.rB, impulse);
	}
}

inline float Physics::ContactConstraint::mixFriction() {
	return Math::sqrt(colliderA->getFriction() * colliderB->getFriction());
}

inline float Physics::ContactConstraint::mixRestitution() {
	return Math::max(colliderA->getRestitution(), colliderB->getRestitution());
}

