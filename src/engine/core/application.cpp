#include "engine/core/application.hpp"


#include "engine/core/memory.hpp"

#include "engine/game/scene-manager.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

Application::Application()
		: keys{0}
		, mouseButtons{0}
		, lastKeys{0}
		, lastMouseButtons{0}
		, mouseX(0.0)
		, mouseY(0.0)
		, lastMouseX(0.0)
		, lastMouseY(0.0)
		, scrollX(0.0)
		, scrollY(0.0) {
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

Application::~Application() {}

void Application::onKeyEvent(WindowHandle window, int key, int scanCode,
		int action, int modifiers) {
	Application::getInstance().keys[key] = action;
}

void Application::onMouseClickEvent(WindowHandle window, int button,
		int action, int modifiers) {
	Application::getInstance().mouseButtons[button] = action;
}

void Application::onMouseMoveEvent(WindowHandle window, double xPos,
		double yPos) {
	Application::getInstance().mouseX = xPos;
	Application::getInstance().mouseY = yPos;
}

void Application::onWindowResizeEvent(WindowHandle windowHandle,
		int width, int height) {
	Application::getInstance().width = width;
	Application::getInstance().height = height;

	Application::getInstance().resize(width, height);
}

void Application::onScrollEvent(WindowHandle windowHandle, double xOffset,
		double yOffset) {
	Application::getInstance().scrollX += xOffset;
	Application::getInstance().scrollY += yOffset;
}

void Application::onWindowCloseEvent(WindowHandle windowHandle) {
	SceneManager::getInstance().stop();
}

