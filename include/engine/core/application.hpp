#pragma once

#include <engine/core/singleton.hpp>

#include <engine/core/base-application.hpp>
#include <engine/game/game-render-context.hpp>

class Application final : public Singleton<Application>,
		public BaseApplication<GameRenderContext> {
	public:
		Application();

		void pollEvents();

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

