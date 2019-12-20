#pragma once

#include <engine/core/input.hpp>
#include <engine/core/monitor.hpp>

class GLFWwindow;
class Application;

typedef GLFWwindow* WindowHandle;

class Window {
	public:
		bool isCloseRequested() const;
		void render();

		void resize(uint32 width, uint32 height);

		void setFullscreen(bool fullscreen);
		void setFullscreen(bool fullscreen, Monitor& monitor);

		void setPosition(uint32 x, uint32 y);
		void moveToCenter();

		void setCursorMode(enum Input::CursorMode cursorMode);

		inline WindowHandle getHandle() { return handle; }
		
		inline uint32 getWidth() const { return width; }
		inline uint32 getHeight() const { return height; }

		inline bool isFullscreen() const { return fullscreen; }

		~Window();
	private:
		NULL_COPY_AND_ASSIGN(Window);

		Window(Application* application, const char* title,
				uint32 width, uint32 height);

		WindowHandle handle;
		uint32 width;
		uint32 height;

		bool fullscreen;
		Monitor* currentMonitor;

		Application* application;

		friend class Application;
};
