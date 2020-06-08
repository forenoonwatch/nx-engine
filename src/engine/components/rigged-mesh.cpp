#include "engine/components/rigged-mesh.hpp"

#include <engine/ecs/ecs.hpp>
#include <engine/rendering/render-system.hpp>

#include <engine/components/transform-component.hpp>

void renderRiggedMeshes(Registry& registry, RenderSystem& renderer) {
    registry.view<TransformComponent, RiggedMesh>().each([&](auto& tfc, auto& rm) {
        if (rm.render) {
            renderer.drawRiggedMesh(*rm.vertexArray, *rm.material, *rm.rig, 
                    tfc.transform.toMatrix());
        }
    });
}
