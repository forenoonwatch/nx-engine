#include "core/application.hpp"

#include "core/memory.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <new>

#define DEFAULT_WINDOW_TITLE	"Window"
#define DEFAULT_WINDOW_WIDTH	1200
#define DEFAULT_WINDOW_HEIGHT	800

ApplicationImpl::ApplicationImpl()
		: handle(nullptr)
		, width(DEFAULT_WINDOW_WIDTH)
		, height(DEFAULT_WINDOW_HEIGHT)
		, fullscreen(false) 
		, currentMonitor(nullptr)
		, monitors(nullptr)
		, numMonitors(0) {
	glfwInit();

	GLFWmonitor** monitorHandles = glfwGetMonitors(&numMonitors);

	monitors = (Monitor*)Memory::malloc(numMonitors * sizeof(Monitor));

	int32 xPos, yPos, width, height;

	for (uint32 i = 0; i < numMonitors; ++i) {
		glfwGetMonitorWorkarea(monitorHandles[i], &xPos, &yPos, &width,
				&height);
		
		new (monitors + i) Monitor(monitorHandles[i], xPos, yPos, width,
				height);
		//monitors[i] = Monitor(monitorHandles[i], xPos, yPos,
		//		width, height);
	}

	currentMonitor = monitors;
	
	//glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	handle = glfwCreateWindow(width, height, DEFAULT_WINDOW_TITLE, nullptr,
			nullptr);

	glfwSetWindowUserPointer(handle, this);

	glfwMakeContextCurrent(handle);
	glewInit();
}

void ApplicationImpl::render() {
	glfwSwapBuffers(handle);
}

void ApplicationImpl::resizeWindow(uint32 width, uint32 height) {
	this->width = width;
	this->height = height;

	glfwSetWindowSize(handle, width, height);
}

void ApplicationImpl::setFullscreen(bool fullscreen) {
	if (fullscreen == this->fullscreen) {
		return;
	}

	this->fullscreen = fullscreen;

	if (fullscreen) {
		currentMonitor = &getPrimaryMonitor();

		glfwSetWindowMonitor(handle, currentMonitor->getHandle(),
				currentMonitor->getX(), currentMonitor->getY(),
				currentMonitor->getWidth(), currentMonitor->getHeight(),
				GLFW_DONT_CARE);
	}
	else {
		glfwSetWindowMonitor(handle, nullptr, currentMonitor->getX()
				+ currentMonitor->getWidth() / 2 - width / 2,
				currentMonitor->getY() + currentMonitor->getHeight() / 2
				- height / 2, width, height, GLFW_DONT_CARE);
	}
}

void ApplicationImpl::setFullscreen(bool fullscreen, Monitor& monitor) {
	if (fullscreen == this->fullscreen && &monitor == currentMonitor) {
		return;
	}

	this->fullscreen = fullscreen;
	currentMonitor = &monitor;

	if (fullscreen) {
		glfwSetWindowMonitor(handle, currentMonitor->getHandle(),
				currentMonitor->getX(), currentMonitor->getY(),
				currentMonitor->getWidth(), currentMonitor->getHeight(),
				GLFW_DONT_CARE);
	}
	else {
		glfwSetWindowMonitor(handle, nullptr, currentMonitor->getX()
				+ currentMonitor->getWidth() / 2 - width / 2,
				currentMonitor->getY() + currentMonitor->getHeight() / 2
				- height / 2, width, height, GLFW_DONT_CARE);
	}
}

void ApplicationImpl::setPosition(uint32 x, uint32 y) {
	glfwSetWindowPos(handle, x, y);
}

void ApplicationImpl::moveToCenter() {
	glfwSetWindowPos(handle, currentMonitor->getX()
			+ currentMonitor->getWidth() / 2 - width / 2,
			currentMonitor->getY() + currentMonitor->getHeight() / 2
			- height / 2);
}

void ApplicationImpl::setCursorMode(enum Input::CursorMode cursorMode) {
	glfwSetInputMode(handle, GLFW_CURSOR, cursorMode);
}

ApplicationImpl::~ApplicationImpl() {
	glfwDestroyWindow(handle);

	Memory::free(monitors);

	glfwTerminate();
}

