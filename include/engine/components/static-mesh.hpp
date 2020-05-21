#pragma once

class VertexArray;
class Material;
class Registry;
class RenderSystem;

struct StaticMesh {
	VertexArray* vertexArray;
	Material* material;
	bool render;
};

void renderStaticMeshes(Registry& registry, RenderSystem& renderer);
