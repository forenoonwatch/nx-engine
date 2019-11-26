#pragma once

#include <engine/core/input.hpp>
#include <engine/core/window.hpp>

#include <functional>

class Application {
	public:
		typedef std::function<void(Window&, uint32, uint32)> ResizeCallback;

		static void init();

		static void pollEvents();

		static bool isKeyDown(enum Input::KeyCode keyCode);
		static bool getKeyPressed(enum Input::KeyCode keyCode);
		static bool getKeyReleased(enum Input::KeyCode keyCode);

		static bool isMouseDown(enum Input::MouseButton mouseButton);
		static bool getMousePressed(enum Input::MouseButton mouseButton);
		static bool getMouseReleased(enum Input::MouseButton mouseButton);

		inline static void setResizeCallback(ResizeCallback callback);

		inline static Monitor& getPrimaryMonitor() { return monitors[0]; }
		inline static Monitor& getMonitor(uint32 i) { return monitors[i]; }

		inline static double getMouseX() { return mouseX; }
		inline static double getMouseY() { return mouseY; }

		inline static double getScrollX() { return scrollX; }
		inline static double getScrollY() { return scrollY; }

		static void destroy();
	protected:
		static void bindInputCallbacks(WindowHandle windowHandle);

		friend class Window;
	private:
		NULL_COPY_AND_ASSIGN(Application);
		Application() = delete;

		static Monitor* monitors;

		static bool keys[Input::KEY_LAST + 1];
		static bool mouseButtons[Input::MOUSE_BUTTON_LAST + 1];

		static bool lastKeys[Input::KEY_LAST + 1];
		static bool lastMouseButtons[Input::MOUSE_BUTTON_LAST + 1];

		static double mouseX;
		static double mouseY;

		static double scrollX;
		static double scrollY;

		static ResizeCallback resizeCallback;

		static void onKeyEvent(WindowHandle, int, int, int, int);
		static void onMouseClickEvent(WindowHandle, int, int, int);
		static void onMouseMoveEvent(WindowHandle, double, double);
		static void onWindowResizeEvent(WindowHandle, int, int);
		static void onScrollEvent(WindowHandle, double, double);
};

inline void Application::setResizeCallback(ResizeCallback callback) {
	resizeCallback = callback;
}
