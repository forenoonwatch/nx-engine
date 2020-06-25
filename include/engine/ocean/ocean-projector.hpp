#pragma once

#include <engine/rendering/camera.hpp>

class Registry;

class OceanProjector {
	public:
		OceanProjector(Camera& camera);

		void update(Camera& camera);

		inline const Vector4f* getCorners() const { return corners; }
	private:
		NULL_COPY_AND_ASSIGN(OceanProjector);

		Camera projectorCamera;
		Matrix4f mProjector;
		Vector4f corners[4];

		static const uint32 cameraEdges[24];
};
