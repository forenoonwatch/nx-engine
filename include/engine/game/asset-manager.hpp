#pragma once

#include <engine/core/common.hpp>
#include <engine/core/array-list.hpp>
#include <engine/core/hash-map.hpp>
#include <engine/core/string.hpp>
#include <engine/core/memory.hpp>

#include <engine/rendering/indexed-model.hpp>

class RenderContext;

class Texture;
class CubeMap;
class VertexArray;
class Shader;

class AssetManager {
	public:
		inline AssetManager(RenderContext* renderContext)
				: renderContext(renderContext) {}

		bool loadStaticMesh(const String& name, const String& fileName,
				const IndexedModel::AllocationHints& hints);
		bool loadStaticMesh(const String& name, const String& modelName,
				const String& fileName, const IndexedModel::AllocationHints& hints);

		bool loadTexture(const String& name, const String& fileName);
		bool loadCubeMap(const String& name, const String* fileNames, bool singleFile);

		bool loadShader(const String& name, const String& fileName);

		inline Texture& getTexture(const String& name) { return *textures[name]; }
		inline CubeMap& getCubeMap(const String& name) { return *cubeMaps[name]; }
		
		inline IndexedModel& getModel(const String& name) { return *models[name]; }
		inline VertexArray& getVertexArray(const String& name) { return *vertexArrays[name]; }
		
		inline Shader& getShader(const String& name) { return *shaders[name]; }

		template <typename T>
		inline String getAssetNameByPointer(Memory::SharedPointer<T> sp) const {
			HashMap<uintptr, String>::const_iterator it = names.find((uintptr)sp.get());
			return it == names.cend() ? String("") : it->second;
		}

		//inline Rig& getRig(const String& name) { return *rigs[name]; }
		//inline Animation& getAnimation(const String& name) { return *animations[name]; }
		//inline Material& getMaterial(const String& name) { return *materials[name]; }
		//inline Font& getFont(const String& name) { return *fonts[name]; }
	private:
		NULL_COPY_AND_ASSIGN(AssetManager);

		RenderContext* renderContext;

		HashMap<String, Memory::SharedPointer<IndexedModel>> models;
		HashMap<String, Memory::SharedPointer<VertexArray>> vertexArrays;

		HashMap<String, Memory::SharedPointer<Texture>> textures;
		HashMap<String, Memory::SharedPointer<CubeMap>> cubeMaps;

		HashMap<String, Memory::SharedPointer<Shader>> shaders;
		
		HashMap<uintptr, String> names;

		//void loadFont(const ArrayList<String>& tokens, FontLoader& fontLoader);
		//HashMap<String, Memory::SharedPointer<Font>> fonts;
		//HashMap<String, Memory::SharedPointer<Material>> materials;
		
		//HashMap<String, Memory::SharedPointer<Rig>> rigs;
		//HashMap<String, Memory::SharedPointer<Animation>> animations;
};
