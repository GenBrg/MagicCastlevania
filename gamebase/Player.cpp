#include "Player.hpp"
#include "../Util.hpp"

#include <SDL.h>

#include <iostream>

Player::Player() :
sprite_(sprites->lookup("dunes-ship")),
movement_component_(transform_)
{
	input_system_.Register(SDLK_a, [this](InputSystem::KeyState key_state, float elapsed) {
		if (key_state.pressed) {
			movement_component_.MoveLeft();
		}
	});

	input_system_.Register(SDLK_d, [this](InputSystem::KeyState key_state, float elapsed) {
		if (key_state.pressed) {
			movement_component_.MoveRight();
		}
	});

	input_system_.Register(SDLK_SPACE, [this](InputSystem::KeyState key_state, float elapsed) {
		if (key_state.pressed) {
			movement_component_.Jump();
		}
	});
}

bool Player::OnKeyEvent(SDL_Event const &evt)
{
	return input_system_.OnKeyEvent(evt);
}

void Player::Update(float elapsed)
{
	input_system_.Update(elapsed);
	movement_component_.Update(elapsed);
}

void Player::Draw(DrawSprites& draw) const
{
	draw.draw(sprite_, transform_);
}
