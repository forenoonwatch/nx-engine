#include "engine/physics/collision/collision.hpp"

#include "engine/physics/dynamics/body.hpp"
#include "engine/physics/dynamics/manifold.hpp"

#include "engine/physics/collision/plane-collider.hpp"
#include "engine/physics/collision/convex-collider.hpp"

void Physics::collisionConvexPlane(Manifold& manifold, Collider& a,
		Collider& b) {
	Body* bodyA = a.getBody();
	Body* bodyB = b.getBody();

	ConvexCollider* conv = (ConvexCollider*)&a;

	Matrix4f tf = b.getWorldTransform().toMatrix();
	Vector3f normal(tf[1]);

	manifold.setNormal(-normal);

	for (uint32 i = 0; i < conv->getVertices().size(); ++i) {
		Vector3f v = a.getWorldTransform().transform(conv->getVertices()[i],
				1.f);

		float d = Math::dot(normal, v - b.getWorldTransform().getPosition());

		if (d < 0.f) {
			FeaturePair fp;
			fp.key = i;

			manifold.addContact(v - normal * d, -d, fp);
		}
	}
}

void Physics::collisionPlaneConvex(Manifold& manifold, Collider& a,
		Collider& b) {
	collisionConvexPlane(manifold, b, a);
	manifold.setNormal(-manifold.getNormal());
}

