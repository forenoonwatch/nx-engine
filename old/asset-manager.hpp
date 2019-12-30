#pragma once

#include <engine/core/common.hpp>
#include <engine/core/singleton.hpp>

#include <engine/core/

class IndexedModel;

class Texture;
class CubeMap;
class Material;
class VertexArray;
class Shader;

// TODO: convert asset manager into singleton with ResourceCaches
class AssetManager final : public Singleton<AssetManager> {
	public:
		inline AssetManager(RenderContext* renderContext)
				: renderContext(renderContext) {}

		bool loadStaticMesh(const uint32 id, const uint32 fileName,
				const IndexedModel::AllocationHints& hints);
		bool loadStaticMesh(const uint32 id, const uint32 modelName,
				const uint32 fileName, const IndexedModel::AllocationHints& hints);

		bool loadTexture(const uint32 id, const uint32 fileName);
		bool loadCubeMap(const uint32 id, const uint32 fileName);
		bool loadCubeMap(const uint32 id, const String* fileNames);
		bool loadMaterial(const uint32 id, const uint32 diffuseFileName,
				const uint32 normalFileName, const uint32 materialFileName);

		bool loadShader(const uint32 id, const uint32 fileName);

		inline Texture& getTexture(const uint32 id) { return *textures[id]; }
		inline CubeMap& getCubeMap(const uint32 id) { return *cubeMaps[id]; }
		inline Material& getMaterial(const uint32 id) { return *materials[id]; }
		
		inline IndexedModel& getModel(const uint32 id) { return *models[id]; }
		inline VertexArray& getVertexArray(const uint32 id) { return *vertexArrays[id]; }
		
		inline Shader& getShader(const uint32 id) { return *shaders[id]; }

		//inline Rig& getRig(const uint32 id) { return *rigs[id]; }
		//inline Animation& getAnimation(const uint32 id) { return *animations[id]; }
		//inline Font& getFont(const uint32 id) { return *fonts[id]; }
	private:
		NULL_COPY_AND_ASSIGN(AssetManager);

		RenderContext* renderContext;

		HashMap<String, Memory::SharedPointer<IndexedModel>> models;
		HashMap<String, Memory::SharedPointer<VertexArray>> vertexArrays;

		HashMap<String, Memory::SharedPointer<Texture>> textures;
		HashMap<String, Memory::SharedPointer<CubeMap>> cubeMaps;
		HashMap<String, Memory::SharedPointer<Material>> materials;

		HashMap<String, Memory::SharedPointer<Shader>> shaders;
		
		HashMap<uintptr, String> ids;

		//void loadFont(const ArrayList<String>& tokens, FontLoader& fontLoader);
		//HashMap<String, Memory::SharedPointer<Font>> fonts;
		
		//HashMap<String, Memory::SharedPointer<Rig>> rigs;
		//HashMap<String, Memory::SharedPointer<Animation>> animations;
};
