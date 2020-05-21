#pragma once

#include <engine/core/service.hpp>

#include <engine/application/input.hpp>
#include <engine/application/monitor.hpp>

class GLFWwindow;
typedef GLFWwindow* WindowHandle;

class Application final : public Service<Application> {
	public:
		Application(const char* title, int width, int height);

		void pollEvents();
		void swapBuffers();

		void resizeWindow(int width, int height);

		void setFullscreen(bool fullscreen);
		void setFullscreen(bool fullscreen, Monitor& monitor);

		void setPosition(uint32 x, uint32 y);
		void moveToCenter();

		void setCursorMode(enum Input::CursorMode cursorMode);

		inline WindowHandle getHandle() { return handle; }

		inline int getWidth() const { return width; }
		inline int getHeight() const { return height; }

		inline bool isFullscreen() const { return fullscreen; }
		
		inline Monitor& getPrimaryMonitor() { return monitors[0]; }
		inline Monitor& getMonitor(uint32 i) { return monitors[i]; }

		inline int32 getNumMonitors() const { return numMonitors; }

		bool isKeyDown(enum Input::KeyCode keyCode) const;
		bool getKeyPressed(enum Input::KeyCode keyCode) const;
		bool getKeyReleased(enum Input::KeyCode keyCode) const;

		bool isMouseDown(enum Input::MouseButton mouseButton) const;
		bool getMousePressed(enum Input::MouseButton mouseButton) const;
		bool getMouseReleased(enum Input::MouseButton mouseButton) const;

		inline double getMouseX() const { return mouseX; }
		inline double getMouseY() const { return mouseY; }

		inline double getMouseDeltaX() const { return mouseX - lastMouseX; }
		inline double getMouseDeltaY() const { return mouseY - lastMouseY; }

		inline double getScrollX() const { return scrollX; }
		inline double getScrollY() const { return scrollY; }

		~Application();
	private:
		NULL_COPY_AND_ASSIGN(Application);

		WindowHandle handle;

		int width;
		int height;

		bool fullscreen;
		Monitor* currentMonitor;

		Monitor* monitors;
		int32 numMonitors;

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
		
		static void onKeyEvent(WindowHandle, int, int, int, int);
		static void onMouseClickEvent(WindowHandle, int, int, int);
		static void onMouseMoveEvent(WindowHandle, double, double);
		static void onWindowResizeEvent(WindowHandle, int, int);
		static void onScrollEvent(WindowHandle, double, double);
		static void onWindowCloseEvent(WindowHandle);
};
