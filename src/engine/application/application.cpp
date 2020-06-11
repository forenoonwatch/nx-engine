#include "engine/application/application.hpp"

#include <engine/core/memory.hpp>

#include <engine/scene/scene-manager.hpp>

#include <engine/rendering/render-system.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <new>

Application::Application(const char* title, int width, int height)
		: handle(nullptr)
		, width(width)
		, height(height)
		, fullscreen(false) 
		, currentMonitor(nullptr)
		, monitors(nullptr)
		, numMonitors(0)

		, keys{0}
		, mouseButtons{0}
		, lastKeys{0}
		, lastMouseButtons{0}
		, mouseX(0.0)
		, mouseY(0.0)
		, lastMouseX(0.0)
		, lastMouseY(0.0)
		, scrollX(0.0)
		, scrollY(0.0) {
	glfwInit();

	GLFWmonitor** monitorHandles = glfwGetMonitors(&numMonitors);

	monitors = (Monitor*)Memory::malloc(numMonitors * sizeof(Monitor));

	int32 xPos, yPos, monWidth, monHeight;

	for (uint32 i = 0; i < numMonitors; ++i) {
		glfwGetMonitorWorkarea(monitorHandles[i], &xPos, &yPos, &monWidth,
				&monHeight);
		
		new (monitors + i) Monitor(monitorHandles[i], xPos, yPos, monWidth,
				monHeight);
		//monitors[i] = Monitor(monitorHandles[i], xPos, yPos,
		//		width, height);
	}

	currentMonitor = monitors;
	
	//glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	handle = glfwCreateWindow(width, height, title, nullptr, nullptr);

	glfwSetWindowUserPointer(handle, this);

	glfwMakeContextCurrent(handle);
	glewInit();

	glfwSetKeyCallback(handle, Application::onKeyEvent);
	glfwSetMouseButtonCallback(handle, Application::onMouseClickEvent);
	glfwSetCursorPosCallback(handle, Application::onMouseMoveEvent);
	glfwSetFramebufferSizeCallback(handle, Application::onWindowResizeEvent);
	glfwSetScrollCallback(handle, Application::onScrollEvent);
	glfwSetWindowCloseCallback(handle, Application::onWindowCloseEvent);
	
	Application::onWindowResizeEvent(handle, getWidth(), getHeight());
}

void Application::pollEvents() {
	Memory::memcpy(lastKeys, keys, sizeof(keys));
	Memory::memcpy(lastMouseButtons, mouseButtons, sizeof(mouseButtons));

	lastMouseX = mouseX;
	lastMouseY = mouseY;
	
	glfwPollEvents();
}

void Application::swapBuffers() {
	glfwSwapBuffers(handle);
}

void Application::resizeWindow(int width, int height) {
	this->width = width;
	this->height = height;

	glfwSetWindowSize(handle, width, height);
}

void Application::setFullscreen(bool fullscreen) {
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

void Application::setFullscreen(bool fullscreen, Monitor& monitor) {
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

void Application::setPosition(uint32 x, uint32 y) {
	glfwSetWindowPos(handle, x, y);
}

void Application::moveToCenter() {
	glfwSetWindowPos(handle, currentMonitor->getX()
			+ currentMonitor->getWidth() / 2 - width / 2,
			currentMonitor->getY() + currentMonitor->getHeight() / 2
			- height / 2);
}

void Application::setCursorMode(enum Input::CursorMode cursorMode) {
	glfwSetInputMode(handle, GLFW_CURSOR, cursorMode);
}

bool Application::isKeyDown(enum Input::KeyCode keyCode) const {
	return keys[keyCode];
}

bool Application::getKeyPressed(enum Input::KeyCode keyCode) const {
	return keys[keyCode] && !lastKeys[keyCode];
}

bool Application::getKeyReleased(enum Input::KeyCode keyCode) const {
	return !keys[keyCode] && lastKeys[keyCode];
}

bool Application::isMouseDown(enum Input::MouseButton mouseButton) const {
	return mouseButtons[mouseButton];
}

bool Application::getMousePressed(enum Input::MouseButton mouseButton) const {
	return mouseButtons[mouseButton] && !lastMouseButtons[mouseButton];
}

bool Application::getMouseReleased(enum Input::MouseButton mouseButton) const {
	return !mouseButtons[mouseButton] && lastMouseButtons[mouseButton];
}

Application::~Application() {
	glfwDestroyWindow(handle);

	Memory::free(monitors);

	glfwTerminate();
}

void Application::onKeyEvent(WindowHandle window, int key, int scanCode,
		int action, int modifiers) {
	Application::ref().keys[key] = action;
}

void Application::onMouseClickEvent(WindowHandle window, int button,
		int action, int modifiers) {
	Application::ref().mouseButtons[button] = action;
}

void Application::onMouseMoveEvent(WindowHandle window, double xPos,
		double yPos) {
	Application::ref().mouseX = xPos;
	Application::ref().mouseY = yPos;
}

void Application::onWindowResizeEvent(WindowHandle windowHandle,
		int width, int height) {
	if (auto app = Application::get(); app) {
		app->width = width;
		app->height = height;
	}

	if (auto renderer = RenderSystem::get(); renderer) {
		renderer->resize(width, height);
	}
}

void Application::onScrollEvent(WindowHandle windowHandle, double xOffset,
		double yOffset) {
	Application::ref().scrollX += xOffset;
	Application::ref().scrollY += yOffset;
}

void Application::onWindowCloseEvent(WindowHandle windowHandle) {
	SceneManager::ref().stop();
}
