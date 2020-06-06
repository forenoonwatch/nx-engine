#pragma once

#include <engine/animation/bone.hpp>

#include <engine/core/hash-map.hpp>

class Rig {
	public:
		static constexpr const size_t MAX_JOINTS = 20;
		static constexpr const size_t MAX_WEIGHTS = 3;

		inline Rig(const Matrix4f& globalInverseTransform)
				: globalInverseTransform(globalInverseTransform)
				, rootBone(0) {}

		void resetBones();
		void calcRootBone();

		inline bool hasBone(const String& name) const { return nameMap.find(name) != std::end(nameMap); }
		inline void addBone(const String& name, const Matrix4f& localTransform);
		
		inline void setBoneParent(const String& parent, const String& child);
		inline void setFinalTransform(uint32 jointIndex, const Matrix4f& transform);

		inline uint32 getBoneIndex(const String& name) { return nameMap[name]; }
		inline Bone& getBone(uint32 i) { return bones[i]; }
		
		inline size_t getNumBones() const { return bones.size(); }
		inline Bone* getRootBone() { return &bones[rootBone]; }
		inline const Matrix4f& getGlobalInverseTransform() const { return globalInverseTransform; }
		inline Matrix4f* getTransformSet() { return transformSet; }
	private:
		Matrix4f globalInverseTransform; // TODO: see if I really need this
		
		uint32 rootBone;

		ArrayList<Bone> bones;
		HashMap<String, uint32> nameMap;
		Matrix4f transformSet[MAX_JOINTS];
};

inline void Rig::setBoneParent(const String& parent, const String& child) {
	bones[nameMap[child]].parent = nameMap[parent];
	bones[nameMap[parent]].children.push_back(nameMap[child]);
}

inline void Rig::addBone(const String& name, const Matrix4f& localTransform) {
	nameMap[name] = bones.size();
	bones.emplace_back(localTransform, name, bones.size());
}

inline void Rig::setFinalTransform(uint32 jointIndex, const Matrix4f& transform) {
	transformSet[jointIndex] = transform;
}
