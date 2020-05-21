#include "engine/networking/input-state.hpp"

#include "engine/game/player-input.hpp"

#include "engine/ecs/registry.hpp"

void writeInputStates(InputState* inputStates) {
	const PlayerInputComponent* pic = ECS::Registry::getInstance()
			.raw<PlayerInputComponent>();
	const size_t numInputs = ECS::Registry::getInstance()
			.size<PlayerInputComponent>();

	for (size_t i = 0; i < numInputs; ++i) {
		// TODO: add client IDs  
		inputStates[i].inputs = pic->forward | (pic->back << 1)
				| (pic->left << 2) | (pic->right << 3) | (pic->jump << 4)
				| (pic->leftMouse << 5) | (pic->rightMouse << 6);

		inputStates[i].mouseDeltaX = pic->mouseDeltaX;
		inputStates[i].mouseDeltaY = pic->mouseDeltaY;

		++pic;
	}
}

