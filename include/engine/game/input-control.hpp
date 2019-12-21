#pragma once

#include <engine/math/math.hpp>

class InputControl {
	public:
		inline InputControl()
				: amount(0.f) {}

		inline void set(float amount) { this->amount = amount; }
		inline void add(float delta) { amount += delta; }

		inline float get() const { return Math::clamp(amount, -1.f, 1.f); }
	private:
		float amount;
};

