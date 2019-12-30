#pragma once

#include <engine/core/input.hpp>
#include <engine/core/monitor.hpp>

class GLFWwindow;
typedef GLFWwindow* WindowHandle;

class ApplicationImpl {
	public:
		ApplicationImpl();
		
		void render();

		void resizeWindow(uint32 width, uint32 height);

		void setFullscreen(bool fullscreen);
		void setFullscreen(bool fullscreen, Monitor& monitor);

		void setPosition(uint32 x, uint32 y);
		void moveToCenter();

		void setCursorMode(enum Input::CursorMode cursorMode);

		inline WindowHandle getHandle() { return handle; }

		inline uint32 getWidth() const { return width; }
		inline uint32 getHeight() const { return height; }

		inline bool isFullscreen() const { return fullscreen; }
		
		inline Monitor& getPrimaryMonitor() { return monitors[0]; }
		inline Monitor& getMonitor(uint32 i) { return monitors[i]; }

		inline int32 getNumMonitors() const { return numMonitors; }

		virtual ~ApplicationImpl();
	protected:
		WindowHandle handle;

		uint32 width;
		uint32 height;

		bool fullscreen;
		Monitor* currentMonitor;

		Monitor* monitors;
		int32 numMonitors;
};

template <typename RenderContext_>
class BaseApplication : public ApplicationImpl, public RenderContext_ {};

