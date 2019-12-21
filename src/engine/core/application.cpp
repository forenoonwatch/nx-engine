#include "core/application.hpp"

#include "core/memory.hpp"

#include "core/application-event-handler.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <new>
#include <stdexcept>

Application* Application::instance(nullptr);

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
		, scrollY(0.0)
		, monitors(nullptr) {
	if (Application::instance) {
		throw std::runtime_error("Attempting to instantiate multiple apps");
	}

	Application::instance = this;

	glfwInit();

	int32 monitorCount;
	GLFWmonitor** monitorHandles = glfwGetMonitors(&monitorCount);

	monitors = (Monitor*)Memory::malloc(monitorCount * sizeof(Monitor));

	int32 xPos, yPos, width, height;

	for (uint32 i = 0; i < monitorCount; ++i) {
		glfwGetMonitorWorkarea(monitorHandles[i], &xPos,
				&yPos, &width, &height);
		
		monitors[i] = Monitor(monitorHandles[i], xPos, yPos,
				width, height);
	}
}

void Application::pollEvents() {
	Memory::memcpy(lastKeys, keys, sizeof(keys));
	Memory::memcpy(lastMouseButtons, mouseButtons, sizeof(mouseButtons));

	lastMouseX = mouseX;
	lastMouseY = mouseY;
	
	glfwPollEvents();
}

Window* Application::createWindow(const char* title, uint32 width,
		uint32 height) {
	Window* window = (Window*)Memory::malloc(sizeof(Window));
	window = new (window) Window(this, title, width, height);

	windows.push_back(window);

	return window;
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
	std::for_each(std::begin(windows), std::end(windows), [](Window* window) {
		Memory::free(window);
	});

	Memory::free(monitors);

	glfwTerminate();
	
	Application::instance = nullptr;
}

void Application::bindInputCallbacks(WindowHandle windowHandle) {
	glfwSetKeyCallback(windowHandle, Application::onKeyEvent);
	glfwSetMouseButtonCallback(windowHandle, Application::onMouseClickEvent);
	glfwSetCursorPosCallback(windowHandle, Application::onMouseMoveEvent);
	glfwSetFramebufferSizeCallback(windowHandle,
			Application::onWindowResizeEvent);
	glfwSetScrollCallback(windowHandle, Application::onScrollEvent);

	const Window* window = (Window*)glfwGetWindowUserPointer(windowHandle);

	Application::onWindowResizeEvent(windowHandle, window->getWidth(),
			window->getHeight());
}

void Application::onKeyEvent(WindowHandle window, int key,
		int scanCode, int action, int modifiers) {
	Application::instance->keys[key] = action;
	
	std::for_each(std::begin(Application::instance->eventHandlers),
			std::end(Application::instance->eventHandlers),
			[&](ApplicationEventHandler* aeh) {
		if (Application::instance->getKeyPressed((Input::KeyCode)key)) {
			aeh->onKeyDown((Input::KeyCode)key);
		}
		else if (Application::instance->getKeyReleased((Input::KeyCode)key)) {
			aeh->onKeyUp((Input::KeyCode)key);
		}
	});
}

void Application::onMouseClickEvent(WindowHandle window,
		int button, int action, int modifiers) {
	Application::instance->mouseButtons[button] = action;
	
	std::for_each(std::begin(Application::instance->eventHandlers),
			std::end(Application::instance->eventHandlers),
			[&](ApplicationEventHandler* aeh) {
		if (Application::instance->getMousePressed(
				(Input::MouseButton)button)) {
			aeh->onMouseDown((Input::MouseButton)button);
		}
		else if (Application::instance->getMouseReleased(
				(Input::MouseButton)button)) {
			aeh->onMouseUp((Input::MouseButton)button);
		}
	});
}

void Application::onMouseMoveEvent(WindowHandle window,
		double xPos, double yPos) {
	Application::instance->mouseX = xPos;
	Application::instance->mouseY = yPos;

	std::for_each(std::begin(Application::instance->eventHandlers),
			std::end(Application::instance->eventHandlers),
			[xPos, yPos](ApplicationEventHandler* aeh) {
		aeh->onMouseMove(xPos, yPos);
	});
}

void Application::onWindowResizeEvent(WindowHandle windowHandle,
		int width, int height) {
	Window* window = (Window*)glfwGetWindowUserPointer(windowHandle);
	
	window->width = (uint32)width;
	window->height = (uint32)height;

	std::for_each(std::begin(Application::instance->eventHandlers),
			std::end(Application::instance->eventHandlers),
			[&](ApplicationEventHandler* aeh) {
		aeh->onWindowResized(*window, (uint32)width, (uint32)height);
	});
}

void Application::onScrollEvent(WindowHandle windowHandle, double xOffset,
		double yOffset) {
	Application::instance->scrollX += xOffset;
	Application::instance->scrollY += yOffset;

	std::for_each(std::begin(Application::instance->eventHandlers),
			std::end(Application::instance->eventHandlers),
			[xOffset, yOffset](ApplicationEventHandler* aeh) {
		aeh->onMouseWheelMove(xOffset, yOffset);
	});
}

