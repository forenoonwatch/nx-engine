#include "engine/physics/collision/collision.hpp"

#include "engine/physics/dynamics/body.hpp"
#include "engine/physics/dynamics/manifold.hpp"

#include "engine/physics/collision/capsule-collider.hpp"

#include <engine/math/math.hpp>
#include <engine/math/distance-gjk.hpp>

#define EPSILON 1e-6f

void Physics::collisionCapsuleCapsule(Manifold& manifold, Collider& a,
		Collider& b) {
	Body* bodyA = a.getBody();
	Body* bodyB = b.getBody();
	
	CapsuleCollider* capsuleA = (CapsuleCollider*)&a;
	CapsuleCollider* capsuleB = (CapsuleCollider*)&b;

	float rSum = capsuleA->getRadius() + capsuleB->getRadius();

	Vector3f resA, resB;
	Math::distanceGJK(capsuleA->getPoints(), 2, capsuleB->getPoints(), 2,
			a.getWorldTransform(), b.getWorldTransform(), resA, resB);

	Vector3f normal = resB - resA;
	float dist = Math::dot(normal, normal);

	if (dist > EPSILON && dist < rSum * rSum) {
		dist = Math::sqrt(dist);

		FeaturePair fp;
		fp.key = 0;

		manifold.setNormal(normal / dist);
		manifold.addContact((resA + resB) * 0.5f, rSum - dist, fp);
	}
}

