#pragma once

class VertexArray;
class Material;
class Rig;
class Registry;
class RenderSystem;

struct RiggedMesh {
    VertexArray* vertexArray;
    Material* material;
    Rig* rig;
    bool render;
};

void renderRiggedMeshes(Registry& registry, RenderSystem& renderer);
