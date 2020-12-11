#include "InputSystem.hpp"

#include <iostream>

void InputSystem::Register(SDL_Keycode keycode, const std::function<void(KeyState&, float)>& action)
{
	input_action_map_[keycode] = action;
}

void InputSystem::Unregister(SDL_Keycode keycode)
{
	input_action_map_.erase(keycode);
}

bool InputSystem::OnKeyEvent(const SDL_Event& evt)
{
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.repeat) {
			//ignore repeats
		} else {
			SDL_Keycode keycode = evt.key.keysym.sym;
			if (input_action_map_.count(keycode) > 0) {
				key_states_[keycode].downs += 1;
				key_states_[keycode].pressed = true;
				key_states_[keycode].released = false;
			}
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		SDL_Keycode keycode = evt.key.keysym.sym;
		if (input_action_map_.count(keycode) > 0) {
			key_states_[keycode].downs += 1;
			key_states_[keycode].pressed = false;
			key_states_[keycode].released = true;
		}
		return true;
	}
	return false;
}

void InputSystem::Update(float elapsed)
{
	for (const auto& [keycode, action] : input_action_map_) {
		action(key_states_[keycode], elapsed);
	}
}

void InputSystem::ClearKeyStates()
{
	for (auto& [_, state] : key_states_) {
		static_cast<void>(_);
		state.Clear();
	}
}

void InputSystem::KeyState::Clear()
{
	downs = 0;
	pressed = 0;
	released = false;
}
