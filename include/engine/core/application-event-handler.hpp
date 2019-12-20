#pragma once

#include <engine/core/common.hpp>
#include <engine/core/input.hpp>

class Window;

class ApplicationEventHandler {
	public:
		ApplicationEventHandler() {}

		virtual void onKeyDown(enum Input::KeyCode keyCode) {}
		virtual void onKeyUp(enum Input::KeyCode keyCode) {}

		virtual void onMouseDown(enum Input::MouseButton mouseButton) {}
		virtual void onMouseUp(enum Input::MouseButton mouseButton) {}

		virtual void onMouseMove(double mousePosX, double mousePosY) {}

		virtual void onMouseWheelMove(double deltaX, double deltaY) {}

		virtual void onWindowResized(Window& window, uint32 newWidth,
				uint32 newHeight) {}

		virtual ~ApplicationEventHandler() {}
	private:
		NULL_COPY_AND_ASSIGN(ApplicationEventHandler);
};

