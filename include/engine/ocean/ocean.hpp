#pragma once

#include <engine/core/service.hpp>

#include <engine/ocean/ocean-fft.hpp>
#include <engine/ocean/ocean-projector.hpp>

#include <engine/math/vector.hpp>

class VertexArray;
class RenderContext;
class Registry;

class Ocean final : public Service<Ocean> {
	public:
		static constexpr const float MAX_AMPLITUDE = 4.f;

		Ocean(RenderContext& context, Camera& camera,
				uint32 gridScale, int32 N, int32 L,
				bool choppy, float timeScale);

		void update(Camera& camera, float deltaTime);
		void render();

		inline OceanFFT& getFFTData() { return oceanFFT; }

		~Ocean();
	private:
		NULL_COPY_AND_ASSIGN(Ocean);

		RenderContext* context;

		OceanFFT oceanFFT;
		OceanProjector projector;

		Shader oceanShader;

		VertexArray* gridArray;

		uint32 gridLength;
};
