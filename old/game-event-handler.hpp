#pragma once

#include <engine/core/singleton.hpp>

#include <engine/core/array-list.hpp>
#include <engine/core/hash-map.hpp>

#include <engine/core/application-event-handler.hpp>

class InputControl;

class GameEventHandler final : public ApplicationEventHandler,
		public Singleton<GameEventHandler>  {
	public:
		inline explicit GameEventHandler()
				: ApplicationEventHandler() {}

		virtual void onKeyDown(enum Input::KeyCode keyCode) override;
		virtual void onKeyUp(enum Input::KeyCode keyCode) override;

		virtual void onMouseDown(enum Input::MouseButton mouseButton) override;
		virtual void onMouseUp(enum Input::MouseButton mouseButton) override;

		virtual void onWindowClosed(Window& window) override;

		void addKeyControl(enum Input::KeyCode keyCode,
				InputControl& inputControl, float weight = 1.f);
	private:
		NULL_COPY_AND_ASSIGN(GameEventHandler);

		static constexpr const uint32 MOUSE_OFFSET = Input::KEY_LAST;

		HashMap<uint32, ArrayList<Pair<float, InputControl&>>> inputs;

		void updateInput(uint32 inputCode, float dir);
};

