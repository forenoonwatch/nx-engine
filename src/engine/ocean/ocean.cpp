#include "engine/ocean/ocean.hpp"

#include <engine/ocean/ocean-projector.hpp>

#include <engine/rendering/vertex-array.hpp>
#include <engine/rendering/uniform-buffer.hpp>
#include <engine/rendering/render-context.hpp>
#include <engine/rendering/render-system.hpp>

#include <engine/ecs/ecs.hpp>

Ocean::Ocean(RenderContext& context, Camera& camera,
			uint32 gridLength, int32 N, int32 L,
			bool choppy, float timeScale)
		: context(&context)
		, oceanFFT(context, N, L, choppy, timeScale)
		, projector(camera)
		, oceanShader(context)
		, gridArray(nullptr)
		, gridLength(gridLength) {
	oceanShader.load("./res/shaders/ocean/ocean-deferred-2.glsl");

	IndexedModel grid;

	const float fGridLength = static_cast<float>(gridLength);
	const float ifGridLength = 1.f / (fGridLength - 1.f);

	grid.allocateElement(2);

	for (float y = 0; y < fGridLength; ++y) {
		for (float x = 0; x < fGridLength; ++x) {
			grid.addElement2f(0, x / (fGridLength - 1.f),
					y / (fGridLength - 1.f));
		}
	}

	for (int32 y = 1; y < gridLength; ++y) {
		for (int32 x = 1; x < gridLength; ++x) {
			int32 i0 = y * gridLength + x;
			int32 i1 = (y - 1) * gridLength + x;
			int32 i2 = y * gridLength + x - 1;
			int32 i3 = (y - 1) * gridLength + x - 1;

			grid.addIndices3i(i0, i1, i2);
			grid.addIndices3i(i1, i3, i2);
		}
	}

	gridArray = new VertexArray(context, grid, GL_STATIC_DRAW);

	//oceanFFT.setFoldingParams(1.f, 1.f, 0.03f);
	oceanFFT.setFoldingParams(1.f, 1.f, 0.035f);
	//oceanFFT.setOceanParams(5.f, Vector2f(1.f, 1.f), 100.f, 0.05f);
	oceanFFT.setOceanParams(1000.f, Vector2f(1.f, 1.f), 100000.f, 0.1f);

	oceanFFT.setTimeScale(1.f);

	auto oceanDataBuffer = RenderContext::ref().getUniformBuffer("OceanData").lock();
	oceanDataBuffer->set({"amplitude", "detailAmplitude", "lambda", "sampleScale"}, 1.5f, 0.01f, 1.f, 0.02f);
	//oceanDataBuffer->set({"amplitude", "detailAmplitude", "lambda", "sampleScale"}, 5.f, 0.01f, 0.1f, 0.005f);

	context.awaitFinish();
}

void Ocean::update(Camera& camera, float deltaTime) { 
	auto oceanDataBuffer = RenderContext::ref().getUniformBuffer("OceanData").lock();

	oceanFFT.update(deltaTime);
	projector.update(camera);
	oceanDataBuffer->update(projector.getCorners(), 4 * sizeof(Vector4f));
}

void Ocean::render() {
	auto& renderer = RenderSystem::ref();

	oceanShader.setSampler("displacementMap", oceanFFT.getDisplacement(),
			renderer.getLinearSampler(), 0);
	oceanShader.setSampler("foldingMap", oceanFFT.getFoldingMap(),
			renderer.getLinearSampler(), 1);

	context->draw(renderer.getTarget(), oceanShader, *gridArray,
			renderer.getDrawParams(), GL_TRIANGLES);
}

Ocean::~Ocean() {
	delete gridArray;
}
