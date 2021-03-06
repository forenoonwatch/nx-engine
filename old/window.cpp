#include "engine/core/window.hpp"

#include "engine/ecs/ecs.hpp"
#include "engine/core/application.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

Window::Window(Application* application, const char* title, uint32 width,
			uint32 height)
		: handle(nullptr)
		, width(width)
		, height(height)
		, fullscreen(false)
		, currentMonitor(&application->getPrimaryMonitor())
		, application(application) {
	//glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	handle = glfwCreateWindow(width, height, title, nullptr, nullptr);

	glfwSetWindowUserPointer(handle, this);

	glfwMakeContextCurrent(handle);
	glewInit();

	Application::bindInputCallbacks(handle);
}

bool Window::isCloseRequested() const {
	return glfwWindowShouldClose(handle);
}

void Window::render() {
	glfwSwapBuffers(handle);
}

void Window::resize(uint32 width, uint32 height) {
	this->width = width;
	this->height = height;

	glfwSetWindowSize(handle, width, height);
}

void Window::setFullscreen(bool fullscreen) {
	if (fullscreen == this->fullscreen) {
		return;
	}

	this->fullscreen = fullscreen;

	if (fullscreen) {
		currentMonitor = &application->getPrimaryMonitor();

		glfwSetWindowMonitor(handle, currentMonitor->getHandle(),
				currentMonitor->getX(), currentMonitor->getY(),
				currentMonitor->getWidth(), currentMonitor->getHeight(),
				GLFW_DONT_CARE);
	}
	else {
		currentMonitor = currentMonitor == nullptr ?
				&application->getPrimaryMonitor() : currentMonitor;

		glfwSetWindowMonitor(handle, nullptr,
				currentMonitor->getX() + currentMonitor->getWidth() / 2
				- width / 2,
				currentMonitor->getY() + currentMonitor->getHeight() / 2
				- height / 2,
				width, height, GLFW_DONT_CARE);
	}
}

void Window::setFullscreen(bool fullscreen, Monitor& monitor) {
	if (fullscreen == this->fullscreen
			&& &monitor == this->currentMonitor) {
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
		glfwSetWindowMonitor(handle, nullptr,
				currentMonitor->getX() + currentMonitor->getWidth() / 2
				- width / 2,
				currentMonitor->getY() + currentMonitor->getHeight() / 2
				- height / 2,
				width, height, GLFW_DONT_CARE);
	}
}

void Window::setPosition(uint32 x, uint32 y) {
	glfwSetWindowPos(handle, x, y);
}

void Window::moveToCenter() {
	glfwSetWindowPos(handle, currentMonitor->getX()
			+ currentMonitor->getWidth() / 2
			- width / 2, currentMonitor->getY()
			+ currentMonitor->getHeight() / 2 - height / 2);
}

void Window::setCursorMode(enum Input::CursorMode cursorMode) {
	glfwSetInputMode(handle, GLFW_CURSOR, cursorMode);
}

Window::~Window() {
	glfwDestroyWindow(handle);
}
