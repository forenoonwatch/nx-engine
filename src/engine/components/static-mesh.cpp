#include "engine/components/static-mesh.hpp"

#include <engine/ecs/ecs.hpp>
#include <engine/rendering/render-system.hpp>

#include <engine/components/transform-component.hpp>

void renderStaticMeshes(Registry& registry, RenderSystem& renderer) {
    registry.view<TransformComponent, StaticMesh>().each([&](auto& tfc, auto& sm) {
        if (sm.render) {
            renderer.drawStaticMesh(*sm.vertexArray, *sm.material, tfc.transform.toMatrix());
        }
    });
}
