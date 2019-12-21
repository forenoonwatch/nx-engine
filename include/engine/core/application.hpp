#pragma once

#include <engine/core/input.hpp>
#include <engine/core/window.hpp>

#include <engine/core/array-list.hpp>

class ApplicationEventHandler;

class Application final {
	public:
		Application();

		void pollEvents();

		Window* createWindow(const char* title, uint32 width, uint32 height);
		
		inline void addEventHandler(ApplicationEventHandler& eventHandler);

		bool isKeyDown(enum Input::KeyCode keyCode) const;
		bool getKeyPressed(enum Input::KeyCode keyCode) const;
		bool getKeyReleased(enum Input::KeyCode keyCode) const;

		bool isMouseDown(enum Input::MouseButton mouseButton) const;
		bool getMousePressed(enum Input::MouseButton mouseButton) const;
		bool getMouseReleased(enum Input::MouseButton mouseButton) const;

		inline Monitor& getPrimaryMonitor() { return monitors[0]; }
		inline Monitor& getMonitor(uint32 i) { return monitors[i]; }

		inline double getMouseX() const { return mouseX; }
		inline double getMouseY() const { return mouseY; }

		inline double getMouseDeltaX() const { return mouseX - lastMouseX; }
		inline double getMouseDeltaY() const { return mouseY - lastMouseY; }

		inline double getScrollX() const { return scrollX; }
		inline double getScrollY() const { return scrollY; }

		~Application();
	protected:
		static void bindInputCallbacks(WindowHandle windowHandle);

		friend class Window;
	private:
		NULL_COPY_AND_ASSIGN(Application);

		bool keys[Input::KEY_LAST + 1];
		bool mouseButtons[Input::MOUSE_BUTTON_LAST + 1];

		bool lastKeys[Input::KEY_LAST + 1];
		bool lastMouseButtons[Input::MOUSE_BUTTON_LAST + 1];

		double mouseX;
		double mouseY;

		double lastMouseX;
		double lastMouseY;

		double scrollX;
		double scrollY;

		ArrayList<ApplicationEventHandler*> eventHandlers;
		
		ArrayList<Window*> windows;
		Monitor* monitors;

		static Application* instance;

		static void onKeyEvent(WindowHandle, int, int, int, int);
		static void onMouseClickEvent(WindowHandle, int, int, int);
		static void onMouseMoveEvent(WindowHandle, double, double);
		static void onWindowResizeEvent(WindowHandle, int, int);
		static void onScrollEvent(WindowHandle, double, double);
};

inline void Application::addEventHandler(
		ApplicationEventHandler& eventHandler) {
	eventHandlers.push_back(&eventHandler);
}
