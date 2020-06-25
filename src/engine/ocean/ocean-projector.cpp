#include "engine/ocean/ocean-projector.hpp"

#include <engine/ocean/ocean.hpp>

#include <engine/ecs/ecs.hpp>
#include <engine/components/camera-component.hpp>

const uint32 OceanProjector::cameraEdges[] = {0, 1,  0, 2,  2, 3,  1, 3,
											  0, 4,  2, 6,  3, 7,  1, 5,
											  4, 6,  4, 5,  5, 7,  6, 7};

OceanProjector::OceanProjector(Camera& camera)
		: projectorCamera(camera)
		, mProjector(1.f) {}

void OceanProjector::update(Camera& camera) {
	Vector3f camPos = camera.getPosition() + camera.getLookVector() * 2.f * Ocean::MAX_AMPLITUDE;

	if (camPos.y < /*Ocean::POSITION.y +*/ Ocean::MAX_AMPLITUDE) {
		camPos.y = /*Ocean::POSITION.y +*/ Ocean::MAX_AMPLITUDE;
	}

	projectorCamera.view = camera.view;
	projectorCamera.view[3] = Vector4f(camPos, 1.f);

	projectorCamera.iView = Math::inverse(projectorCamera.view);
	projectorCamera.viewProjection = projectorCamera.projection
			* projectorCamera.iView;
	projectorCamera.iViewProjection =
			Math::inverse(projectorCamera.viewProjection);

	Vector3f frustum[8];

	float minX = FLT_MAX, maxX = -FLT_MAX;
	float minY = FLT_MAX, maxY = -FLT_MAX;
	bool lookingAtOcean = false;

	for (int32 z = -1, i = 0; z <= 1; z += 2) {
		for (int32 y = -1; y <= 1; y += 2) {
			for (int32 x = -1; x <= 1; x += 2) {
				Vector4f p = projectorCamera.iViewProjection
						* Vector4f(x, y, z, 1.f);
				p /= p.w;
				frustum[i++] = Vector3f(p);

				if (p.y >= -Ocean::MAX_AMPLITUDE
						&& p.y <= Ocean::MAX_AMPLITUDE) {
					if (p.x < minX) {
						minX = p.x;
					}

					if (p.x > maxX) {
						maxX = p.x;
					}

					if (p.y < minY) {
						minY = p.y;
					}

					if (p.y > maxY) {
						maxY = p.y;
					}

					lookingAtOcean = true;
				}
			}
		}
	}

	for (uint32 i = 0;
			i < ARRAY_SIZE_IN_ELEMENTS(OceanProjector::cameraEdges);
			i += 2) {
		const Vector3f& a = frustum[OceanProjector::cameraEdges[i]];
		const Vector3f& b = frustum[OceanProjector::cameraEdges[i + 1]];
		
		Vector3f v = b - a;
		const float m = Math::dot(v, v);

		for (float j = -1.f; j <= 1.f; j += 2.f) {
			const float t = (/*Ocean::POSITION.y + */
					j * Ocean::MAX_AMPLITUDE - a.y) / v.y;

			if (t >= 0.f && t <= m) {
				Vector4f p = projectorCamera.viewProjection
					* Vector4f(a + v * t, 1.f);

				if (p.w <= 0.0001f) {
					continue;
				}

				p /= p.w;

				if (p.x < minX) {
					minX = p.x;
				}

				if (p.x > maxX) {
					maxX = p.x;
				}

				if (p.y < minY) {
					minY = p.y;
				}

				if (p.y > maxY) {
					maxY = p.y;
				}

				lookingAtOcean = true;
			}
		}
	}

	if (lookingAtOcean) {
		minX = minX < -1.f ? -1.f : minX;
		maxX = maxX > 1.f ? 1.f : maxX;
		minY = minY < -1.f ? -1.f : minY;
		maxY = maxY > 1.f ? 1.f : maxY;

		Matrix4f mRange(1.f);

		mRange[0][0] = maxX - minX;
		mRange[1][1] = maxY - minY;
		mRange[3][0] = minX;
		mRange[3][1] = minY;

		mProjector = projectorCamera.iViewProjection * mRange;

		uint32 i = 0;
		for (float y = 0.f; y <= 1.f; ++y) {
			for (float x = 0.f; x <= 1.f; ++x) {
				const Vector4f a = mProjector
						* Vector4f(x, y, -1.f, 1.f);
				const Vector4f b = mProjector
						* Vector4f(x, y, 1.f, 1.f);
				const Vector4f v = b - a;
				const float t = (/* a.w * Ocean::POSITION.y */ -a.y)
					/ (v.y /* - v.w * Ocean::POSITION.y */);

				corners[i++] = a + v * t;
			}
		}
	}
}
