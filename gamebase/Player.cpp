#include "Player.hpp"
#include "../Util.hpp"
#include "Room.hpp"

#include <SDL.h>

#include <iostream>

Player::Player(Room* room) :
transform_(nullptr),
movement_component_(glm::vec4(0.0f, 0.0f, 20.0f, 50.0f), transform_),
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

	input_system_.Register(SDLK_j, [this, room](InputSystem::KeyState key_state, float elapsed) {
		if (key_state.pressed) {
			Attack(room);
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

	invulnerable_countdown_ -= elapsed;
	if (invulnerable_countdown_ < 0.0f) {
		invulnerable_countdown_ = 0.0f;
	}
}

void Player::Draw(DrawSprites& draw) const
{
	draw.draw(sprite_, transform_);
}

void Player::SetPosition(const glm::vec2 &pos) {
	transform_.position_ = pos;
}

void Player::Reset() {
	SetPosition(glm::vec2(20.0f, 113.0f));
	hp_ = 100;
}

void Player::TakeDamage(int attack)
{
	take_damage_guard_(kStiffnessTime, [&](){
		if (invulnerable_countdown_ <= 0.0f) {
			std::cout << "Player take damage!" << std::endl;

			int damage = 1;

			if (attack > defense_) {
				damage = attack - defense_;
			}

			hp_ -= damage;

			invulnerable_countdown_ = kStiffnessTime;

			if (hp_ <= 0) {
				Reset();
			}
		}
	});
}

void Player::Attack(Room* room)
{
	attack_guard_(kAttackCooldown, [&](){
		glm::vec2 initial_pos = transform_.position_ + glm::vec2(20.0f, 0.0f) * transform_.scale_;
		glm::vec2 velocity = glm::vec2(200.0f, 0.0f) * transform_.scale_;
<<<<<<< HEAD
		room->AddPlayerAOE(new AOE(glm::vec4(0.0f, 0.0f, 55.0f, 66.0f), &sprites->lookup("ghost_idle_1"), velocity, 3.0f, attack_, initial_pos, nullptr));
	});
=======
		room->AddPlayerAOE(new AOE(glm::vec4(0.0f, 0.0f, 55.0f, 66.0f), &sprites->lookup("fire_1"), velocity, 3.0f, attack_, initial_pos, nullptr));
	}
>>>>>>> 52a6a96eec0398add67d42bb5364723fa2bbca25
}