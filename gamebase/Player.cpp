#include "Player.hpp"
#include "../Util.hpp"

#include <SDL.h>

#include <iostream>

Player::Player() :
transform_(nullptr),
movement_component_(glm::vec4(0.0f, 0.0f, 15.0f, 24.0f), transform_),
sprite_(sprites->lookup("player_walk_1"))
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

void Player::Update(float elapsed, const std::vector<Collider*>& colliders_to_consider)
{
	input_system_.Update(elapsed);
	movement_component_.Update(elapsed, colliders_to_consider);
}

void Player::Draw(DrawSprites& draw) const
{
	draw.draw(sprite_, transform_);
}

void Player::SetPosition(const glm::vec2 &pos) {
	transform_.position_ = pos;
}
