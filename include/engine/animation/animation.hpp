#pragma once

#include <engine/core/array-list.hpp>

#include <engine/animation/key-frame.hpp>

class Animation {
	public:
		inline Animation(const String& name)
			: name(name) {}

		//inline Animation(const Animation& animation)
		//	: name(animation.name)
		//	, keyFrames(std::move(animation.keyFrames)) {}

		void addKeyFrame(const KeyFrame& keyFrame);

		inline KeyFrame& getKeyFrame(uint32 i) { return keyFrames[i]; }
		inline const KeyFrame& getKeyFrame(uint32 i) const { return keyFrames[i]; }
		inline uint32 getNumFrames() const { return keyFrames.size(); }

		inline const String& getName() const { return name; }
	private:
		String name;
		ArrayList<KeyFrame> keyFrames;
};
