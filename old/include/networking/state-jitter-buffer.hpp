#pragma once

#include <engine/networking/state-update.hpp>
#include <engine/networking/sequence-buffer.hpp>

class StateJitterBuffer {
	public:
		static constexpr const size_t NUM_STATE_UPDATES = 256;
		static constexpr const float DEFAULT_PLAYOUT_DELAY = 5.0 / 60.0;

		explicit StateJitterBuffer(float playoutDelay = DEFAULT_PLAYOUT_DELAY);

		void addStateUpdate(double time, uint16 sequence,
				const StateUpdate& stateUpdate);

		bool getStateUpdate(double time, StateUpdate& stateUpdate);

		void reset(); 
		
		bool isRunning(double time) const;
	private:
		NULL_COPY_AND_ASSIGN(StateJitterBuffer);

		SequenceBuffer<StateUpdate> stateUpdates;

		double startTime;
		float playoutDelay;

		bool stopped;
};

