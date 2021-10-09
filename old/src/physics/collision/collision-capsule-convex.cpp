#include "engine/physics/collision/collision.hpp"

#include "engine/physics/dynamics/body.hpp"
#include "engine/physics/dynamics/manifold.hpp"

#include "engine/physics/collision/capsule-collider.hpp"
#include "engine/physics/collision/convex-collider.hpp"

#include <engine/math/math.hpp>
#include <engine/math/distance-gjk.hpp>

#define EPSILON 1e-6f

void Physics::collisionCapsuleConvex(Manifold& manifold, Collider& a,
		Collider& b) {
	Body* bodyA = a.getBody();
	Body* bodyB = b.getBody();
	
	CapsuleCollider* capsule = (CapsuleCollider*)&a;
	ConvexCollider* convex = (ConvexCollider*)&b;

	Vector3f resA, resB;
	Math::distanceGJK(capsule->getPoints(), 2, &convex->getVertices()[0],
			convex->getVertices().size(), a.getWorldTransform(),
			b.getWorldTransform(), resA, resB);

	Vector3f normal = resB - resA;
	float dist = Math::dot(normal, normal);

	if (dist > EPSILON) {
		if (dist < capsule->getRadius() * capsule->getRadius()) {
			dist = Math::sqrt(dist);
			manifold.setNormal(normal / dist);

			FeaturePair fp;
			fp.key = 0;
			manifold.addContact(resB, capsule->getRadius() - dist, fp);
		}
	}
}

void Physics::collisionConvexCapsule(Manifold& manifold, Collider& a,
		Collider& b) {
	collisionCapsuleConvex(manifold, b, a);
	manifold.setNormal(-manifold.getNormal());
}

