#include "engine/physics/debug-renderer.hpp"

#include <engine/rendering/render-context.hpp>
#include <engine/rendering/render-system.hpp>
#include <engine/rendering/vertex-array.hpp>

#include <engine/physics/physics-util.hpp>

PhysicsDebugRenderer::PhysicsDebugRenderer()
		: shader(RenderContext::ref()) {
	shader.load("./res/shaders/line-shader.glsl");

	IndexedModel model;
	model.allocateElement(3);
	model.allocateElement(3);

	lineVAO = new VertexArray(RenderContext::ref(), model, GL_STREAM_DRAW);

	drawParams.depthFunc = DrawParams::DRAW_FUNC_ALWAYS;
	drawParams.faceCullMode = DrawParams::FACE_CULL_NONE;
}

void PhysicsDebugRenderer::drawLine(const btVector3& from,
		const btVector3& to, const btVector3& color) {
	linePoints.push_back(Physics::btToNativeVec3(from));
	linePoints.push_back(Physics::btToNativeVec3(to));

	lineColors.push_back(Physics::btToNativeVec3(color));
	lineColors.push_back(Physics::btToNativeVec3(color));

	lineIndices.push_back(lineIndices.size());
	lineIndices.push_back(lineIndices.size());
}

void PhysicsDebugRenderer::drawContactPoint(const btVector3& pointOnB,
		const btVector3& normalOnB, btScalar distance, int lifeTime,
		const btVector3& color) {
}

void PhysicsDebugRenderer::reportErrorWarning(const char* warningString) {
	DEBUG_LOG("Physics", LOG_ERROR, warningString);
}

void PhysicsDebugRenderer::draw3dText(const btVector3& location,
		const char* textString) {
}

void PhysicsDebugRenderer::setDebugMode(int debugMode) {
	this->debugMode = debugMode;
}

int PhysicsDebugRenderer::getDebugMode() const {
	return debugMode;
}

void PhysicsDebugRenderer::flush() {
	auto& context = RenderContext::ref();
	auto& renderer = RenderSystem::ref();

	lineVAO->updateBuffer(0, linePoints.data(), linePoints.size() * sizeof(Vector3f));
	lineVAO->updateBuffer(1, lineColors.data(), lineColors.size() * sizeof(Vector3f));
	lineVAO->updateIndexBuffer(lineIndices.data(), lineIndices.size());

	context.draw(renderer.getScreen(), shader, *lineVAO, drawParams, GL_LINES);

	linePoints.clear();
	lineIndices.clear();
}

PhysicsDebugRenderer::~PhysicsDebugRenderer() {
	delete lineVAO;
}
