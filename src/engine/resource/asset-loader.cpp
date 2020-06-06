#include "engine/resource/asset-loader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <engine/math/math.hpp>

namespace {
	struct VertexBoneData {
		uint32 vertexIDs[Rig::MAX_WEIGHTS] = {0};
		float weights[Rig::MAX_WEIGHTS] = {0};
	};

	void initStaticMesh(IndexedModel& newModel, const aiMesh* mesh);
	void initRiggedMesh(IndexedModel& newModel, const aiMesh* mesh, Rig& rig);

	void loadBones(Rig& rig, const aiMesh* mesh, ArrayList<VertexBoneData>& bones);
	void calcRigHierarchy(Rig& rig, const aiNode* rootNode);

	bool initAnimation(Animation& newAnim, const aiAnimation* anim);
};

#define HASH_TIME(time) \
	static_cast<int32>(1e4 * (time))

bool AssetLoader::loadAssets(const StringView& fileName, ArrayList<IndexedModel>& models,
		ArrayList<Rig>& rigs, ArrayList<Animation>& animations) {
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(fileName.data(),
			aiProcess_Triangulate | aiProcess_GenSmoothNormals
			| aiProcess_FlipUVs | aiProcess_CalcTangentSpace
			| aiProcess_JoinIdenticalVertices);

	if (!scene) {
		DEBUG_LOG("Asset Loader", LOG_ERROR, "Failed to load assets from %s", fileName.data());
		return false;
	}

	const Matrix4f globalInverseTransform = Math::rotate(Matrix4f(1.f), MATH_HALF_PI, Vector3f(-1.f, 0.f, 0.f));

	if (scene->HasMeshes()) {
		for (uint32 i = 0; i < scene->mNumMeshes; ++i) {
			const aiMesh* mesh = scene->mMeshes[i];
			IndexedModel newModel;

			if (mesh->HasBones()) {
				rigs.emplace_back(globalInverseTransform);
				auto& rig = rigs.back();

				::initRiggedMesh(newModel, mesh, rig);
				::calcRigHierarchy(rig, scene->mRootNode);

				rig.calcRootBone();
			}
			else {
				::initStaticMesh(newModel, mesh);
			}

			for (uint32 j = 0; j < mesh->mNumFaces; ++j) {
				const aiFace& face = mesh->mFaces[j];

				newModel.addIndices3i(face.mIndices[0],
						face.mIndices[1], face.mIndices[2]);
			}

			models.push_back(newModel);
		}
	}

	if (scene->HasAnimations()) {
		for (uint32 i = 0; i < scene->mNumAnimations; ++i) {
			animations.emplace_back(String(scene->mAnimations[i]->mName.data));

			if (!initAnimation(animations.back(), scene->mAnimations[i])) {
				DEBUG_LOG("Animation Loader", LOG_ERROR,  "Animation load failed!: %s - %s",
						fileName.data(), scene->mAnimations[i]->mName.C_Str());
				return false;
			}
		}
	}

	return true;
}

namespace {
	void initStaticMesh(IndexedModel& newModel, const aiMesh* mesh) {
		const aiVector3D aiZeroVector(0.f, 0.f, 0.f);

		newModel.initStaticMesh();

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

	void initRiggedMesh(IndexedModel& newModel, const aiMesh* mesh, Rig& rig) {
		const aiVector3D aiZeroVector(0.f, 0.f, 0.f);


		ArrayList<VertexBoneData> bones;
		bones.resize(mesh->mNumVertices);

		::loadBones(rig, mesh, bones);

		newModel.initRiggedMesh();

		for (uint32 i = 0; i < mesh->mNumVertices; ++i) {
			const aiVector3D pos = mesh->mVertices[i];
			const aiVector3D normal = mesh->mNormals[i];
			const aiVector3D texCoord = mesh->HasTextureCoords(0)
					? mesh->mTextureCoords[0][i] : aiZeroVector;
			const aiVector3D tangent = mesh->mTangents[i];
			const aiVector3D biTangent = mesh->mBitangents[i];

			const uint32* vertexIDs = bones[i].vertexIDs;
			const float* weights = bones[i].weights;

			newModel.addElement3f(0, pos.x, pos.y, pos.z);
			newModel.addElement2f(1, texCoord.x, texCoord.y);
			newModel.addElement3f(2, normal.x, normal.y, normal.z);
			newModel.addElement3f(3, tangent.x, tangent.y, tangent.z);
			newModel.addElement3f(4, biTangent.x, biTangent.y, biTangent.z);

			newModel.addElement3f(5, vertexIDs[0], vertexIDs[1], vertexIDs[2]);
			newModel.addElement3f(6, weights[0], weights[1], weights[2]);
		}
	}

	void addBoneData(VertexBoneData& vbd, uint32 boneIndex, float weight) {
		for (uint32 i = 0; i < Rig::MAX_WEIGHTS; ++i) {
			if (vbd.weights[i] == 0.f) {
				vbd.vertexIDs[i] = boneIndex;
				vbd.weights[i] = weight;

				return;
			}
		}
	}

	void convertVec3(Vector3f& dest, const aiVector3D& src) {
		dest.x = src.x;
		dest.y = src.y;
		dest.z = src.z;
	}

	void convertQuat(Quaternion& dest, const aiQuaternion& src) {
		dest.x = src.x;
		dest.y = src.y;
		dest.z = src.z;
		dest.w = src.w;
	}

	void convertMatrix(Matrix4f& dest, const aiMatrix4x4& src) {
		for (uint32 y = 0; y < 4; ++y) {
			for (uint32 x = 0; x < 4; ++x) {
				dest[y][x] = src[y][x];
			}
		}
	}

	void loadBones(Rig& rig, const aiMesh* mesh, ArrayList<VertexBoneData>& bones) {
		for (uint32 i = 0; i < mesh->mNumBones; ++i) {
			const auto* bone = mesh->mBones[i];

			uint32 boneIndex = 0;
			String boneName(bone->mName.data);

			if (rig.hasBone(boneName)) {
				boneIndex = rig.getBoneIndex(boneName);
			}
			else {
				boneIndex = rig.getNumBones();

				Matrix4f m;
				::convertMatrix(m, bone->mOffsetMatrix);

				rig.addBone(boneName, m);
			}

			for (uint32 j = 0; j < bone->mNumWeights; ++j) {
				const auto& vertexWeight = bone->mWeights[j];
				::addBoneData(bones[vertexWeight.mVertexId], boneIndex, vertexWeight.mWeight);
			}
		}
	}

	void calcRigHierarchy(Rig& rig, const aiNode* rootNode) {
		const String parentName(rootNode->mName.data);
		const bool isParent = rig.hasBone(parentName);

		for (uint32 i = 0; i < rootNode->mNumChildren; ++i) {
			if (isParent) {
				const String boneName(rootNode->mChildren[i]->mName.data);

				if (rig.hasBone(boneName)) {
					rig.setBoneParent(parentName, boneName);
				}
			}

			::calcRigHierarchy(rig, rootNode->mChildren[i]);
		}
	}

	bool initAnimation(Animation& newAnim, const aiAnimation* anim) {
		HashMap<int32, KeyFrame> keyFrames;

		for (uint32 i = 0; i < anim->mNumChannels; ++i) {
			const auto* channel = anim->mChannels[i];
			const String boneName(channel->mNodeName.data);

			/*if (channel->mNumPositionKeys != channel->mNumRotationKeys
					|| channel->mNumScalingKeys != channel->mNumRotationKeys
					|| channel->mNumPositionKeys != channel->mNumScalingKeys) {
				DEBUG_LOG("Animation Loader", LOG_ERROR, "Animation load error - non-equivalent amount of keys");
				return false;
			}*/

			for (uint32 j = 0; j < channel->mNumRotationKeys; ++j) {
				float time = channel->mRotationKeys[j].mTime;

				/*if (!epsilonEq(time - channel->mPositionKeys[j].mTime)
						|| !epsilonEq(time - channel->mScalingKeys[j].mTime)) {
					return false;
				}*/

				time /= anim->mTicksPerSecond;

				auto frame = keyFrames.find(HASH_TIME(time));

				if (frame == keyFrames.end()) {
					KeyFrame keyFrame(time);

					Vector3f pos, scale;
					Quaternion rot;

					::convertVec3(pos, channel->mPositionKeys[j].mValue);
					::convertQuat(rot, channel->mRotationKeys[j].mValue);
					::convertVec3(scale, channel->mScalingKeys[j].mValue);

					keyFrame.addBoneTransform(boneName, Transform(pos, rot, scale));
					keyFrames.emplace(HASH_TIME(time), keyFrame); // TODO: see if some kind of move operation is faster
				}
				else {
					Vector3f pos, scale;
					Quaternion rot;

					::convertVec3(pos, channel->mPositionKeys[j].mValue);
					::convertQuat(rot, channel->mRotationKeys[j].mValue);
					::convertVec3(scale, channel->mScalingKeys[j].mValue);

					frame->second.addBoneTransform(boneName, Transform(pos, rot, scale));
				}
			}
		}

		for (auto& pair : keyFrames) {
			newAnim.addKeyFrame(pair.second);
		}

		return true;
	}
};
