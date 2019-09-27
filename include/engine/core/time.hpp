#pragma once

#include "engine/core/common.hpp"

#include <GLFW/glfw3.h>

namespace Time {
	FORCEINLINE double getTime() {
		return glfwGetTime();
	}
}
