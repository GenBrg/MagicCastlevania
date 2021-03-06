#pragma once

#include "Singleton.hpp"

#include <SDL.h>

#include <unordered_map>
#include <string>
#include <functional>

class InputSystem;

/**
 * Input system that maps each key code to action.
 * @author Jiasheng Zhou
 */
class InputSystem : public Singleton<InputSystem> {
public:
	struct KeyState {
		uint8_t downs = 0;
		uint8_t pressed = 0;
		bool released = false;

		void Clear();
	};

	void Register(SDL_Keycode keycode, const std::function<void(KeyState&, float)>& action);
	void Unregister(SDL_Keycode keycode);

	bool OnKeyEvent(const SDL_Event& evt);
	void Update(float elapsed);
	void ClearKeyStates();

private:
	std::unordered_map<SDL_Keycode, std::function<void(KeyState&, float)>> input_action_map_;
	std::unordered_map<SDL_Keycode, KeyState> key_states_;
};
