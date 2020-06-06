#include "engine/animation/animation.hpp"

#include <algorithm>

void Animation::addKeyFrame(const KeyFrame& keyFrame) {
	keyFrames.push_back(keyFrame);

	std::sort(std::begin(keyFrames), std::end(keyFrames), [&](auto a, auto b) {
		return a.getTime() < b.getTime();
	});
}
