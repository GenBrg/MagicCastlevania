#pragma once

#include <SDL.h>

#include <unordered_map>
#include <string>
#include <functional>

class InputSystem {
public:
	static InputSystem& Instance()
	{
		static InputSystem input_system;
		return input_system;
	}

	void RegisterInput(SDL_KeyCode keycode, const std::string& action_name);
	void RegisterAction(const std::string& action_name, const std::function<void()>& action);
	void UnregisterInput(SDL_KeyCode keycode);
	void UnregisterAction(const std::string& action_name);

	void Update(const SDL_Event& evt);

private:
	InputSystem() {}

	std::unordered_map<SDL_KeyCode, std::string> input_map_;
	std::unordered_map<std::string, std::function<void()>> action_map_;
};