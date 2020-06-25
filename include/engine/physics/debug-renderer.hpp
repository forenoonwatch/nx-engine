#pragma once

#include <engine/core/array-list.hpp>

#include <engine/rendering/draw-params.hpp>
#include <engine/rendering/shader.hpp>

#include <engine/math/vector.hpp>

#include <bullet/LinearMath/btIDebugDraw.h>

class VertexArray;

class PhysicsDebugRenderer final : public btIDebugDraw {
	public:
		PhysicsDebugRenderer();

		virtual void drawLine(const btVector3& from, const btVector3& to,
				const btVector3& color) override;

		virtual void drawContactPoint(const btVector3& pointOnB,
				const btVector3& normalOnB, btScalar distance, int lifeTime,
				const btVector3& color) override;

		virtual void reportErrorWarning(const char* warningString) override;

		virtual void draw3dText(const btVector3& location,
				const char* textString) override;

		virtual void setDebugMode(int debugMode) override;
		virtual int getDebugMode() const override;

		void flush();

		virtual ~PhysicsDebugRenderer();
	private:
		DrawParams drawParams;
		
		int debugMode;

		Shader shader;

		VertexArray* lineVAO;

		ArrayList<Vector3f> linePoints;
		ArrayList<Vector3f> lineColors;
		ArrayList<uint32> lineIndices;
};
