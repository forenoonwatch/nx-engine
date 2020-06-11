#pragma once

#include <engine/core/service.hpp>

#include <engine/resource/resource-cache.hpp>

class Texture;
class CubeMap;
class Material;
class Shader;
class IndexedModel;
class VertexArray;
class Rig;
class Animation;
class Font;

class ResourceManager final : public Service<ResourceManager> {
    public:
        ResourceCache<Texture> textures;
        ResourceCache<CubeMap> cubeMaps;
        ResourceCache<Material> materials;
        ResourceCache<Shader> shaders;
        ResourceCache<IndexedModel> models;
        ResourceCache<VertexArray> vertexArrays;
        ResourceCache<Rig> rigs;
        ResourceCache<Animation> animations;
        ResourceCache<Font> fonts;
    private:
};
