#include "engine/resource/asset-loader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static void initStaticMesh(IndexedModel& newModel, const aiMesh* mesh);

bool AssetLoader::loadStaticMeshes(const StringView& fileName, ArrayList<IndexedModel>& models) {
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(fileName.data(),
			aiProcess_Triangulate | aiProcess_GenSmoothNormals
			| aiProcess_FlipUVs | aiProcess_CalcTangentSpace
			| aiProcess_JoinIdenticalVertices);

	if (!scene) {
		DEBUG_LOG("Asset Loader", LOG_ERROR, "Failed to load assets from %s", fileName.data());
		return false;
	}

	IndexedModel::AllocationHints hints;
		hints.elementSizes.push_back(3);
		hints.elementSizes.push_back(2);
		hints.elementSizes.push_back(3);
		hints.elementSizes.push_back(3);
		hints.elementSizes.push_back(3);
		hints.elementSizes.push_back(16);
		hints.instancedElementStartIndex = 5;

	for (uint32 i = 0; i < scene->mNumMeshes; ++i) {
		const aiMesh* mesh = scene->mMeshes[i];
		IndexedModel newModel(hints);

		initStaticMesh(newModel, mesh);

		for (uint32 j = 0; j < mesh->mNumFaces; ++j) {
			const aiFace& face = mesh->mFaces[j];

			newModel.addIndices3i(face.mIndices[0],
					face.mIndices[1], face.mIndices[2]);
		}

		models.push_back(newModel);
	}

	return true;
}

static void initStaticMesh(IndexedModel& newModel, const aiMesh* mesh) {
	const aiVector3D aiZeroVector(0.f, 0.f, 0.f);

	for (uint32 i = 0; i < mesh->mNumVertices; ++i) {
		const aiVector3D pos = mesh->mVertices[i];
		const aiVector3D normal = mesh->mNormals[i];
		const aiVector3D texCoord = mesh->HasTextureCoords(0)
				? mesh->mTextureCoords[0][i] : aiZeroVector;
		const aiVector3D tangent = mesh->mTangents[i];
		const aiVector3D biTangent = mesh->mBitangents[i];

		newModel.addElement3f(0, pos.x, pos.y, pos.z);
		newModel.addElement2f(1, texCoord.x, texCoord.y);
		newModel.addElement3f(2, normal.x, normal.y, normal.z);
		newModel.addElement3f(3, tangent.x, tangent.y, tangent.z);
		newModel.addElement3f(4, biTangent.x, biTangent.y, biTangent.z);
	}
}
