#include "engine/networking/state-jitter-buffer.hpp"

#include <engine/core/memory.hpp>
#include <engine/math/math.hpp>

StateJitterBuffer::StateJitterBuffer(float playoutDelay)
		: stateUpdates(NUM_STATE_UPDATES)
		, startTime(0.0)
		, playoutDelay(playoutDelay)
		, stopped(true) {}

void StateJitterBuffer::addStateUpdate(double time, uint16 sequence,
		const StateUpdate& stateUpdate) {
	if (stopped) {
		stopped = false;
		startTime = time;
	}

	if (auto entry = stateUpdates.insert(sequence); entry) {
		Memory::memcpy(entry, &stateUpdate, sizeof(stateUpdate));
	}
}

bool StateJitterBuffer::getStateUpdate(double time, StateUpdate& stateUpdate) {
	if (stopped) {
		return false;
	}

	time -= startTime + playoutDelay;

	if (time < 0.0) {
		return false;
	}

	const uint16 seq = (uint16)( (uint64)std::floor(time * 60) );

	if (const auto entry = stateUpdates.find(seq); entry) {
		Memory::memcpy(&stateUpdate, entry, sizeof(stateUpdate));
		stateUpdates.remove(seq);

		return true;
	}

	return false;
}

void StateJitterBuffer::reset() {
	stopped = true;
	startTime = 0.0;
	stateUpdates.reset();
}

bool StateJitterBuffer::isRunning(double time) const {
	if (stopped) {
		return false;
	}

	return (time - (startTime + playoutDelay)) >= 0.0;
}

