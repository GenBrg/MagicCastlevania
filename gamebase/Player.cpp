#include "Player.hpp"
#include "../Util.hpp"
#include "../Load.hpp"
#include "../engine/Timer.hpp"
#include "Room.hpp"

#include <SDL.h>

#include <iostream>
#include <fstream>
#include <stdexcept>

// Needs to load animation after all sprites are loaded.
Load<void> load_player_config(LoadTagLate, [](){
	Player::LoadConfig(data_path("player.config"));
});

const std::unordered_map<std::string, Player::AnimationState> Player::kAnimationNameStateMap 
{
	{ "stand", Player::AnimationState::STILL },
	{ "walk", Player::AnimationState::WALK },
	{ "jump", Player::AnimationState::JUMP },
	{ "fall", Player::AnimationState::FALL },
	{ "hurt", Player::AnimationState::HURT },
	{ "death", Player::AnimationState::DEATH },
	{ "attack", Player::AnimationState::ATTACK }
};

Player::Player(Room* room) :
transform_(nullptr),
movement_component_(glm::vec4(0.0f, 0.0f, 20.0f, 50.0f), transform_),
animation_controller_(&transform_)
{
	input_system_.Register(SDLK_a, [this](InputSystem::KeyState& key_state, float elapsed) {
		if (key_state.pressed) {
			movement_component_.MoveLeft();
		}
	});

	input_system_.Register(SDLK_d, [this](InputSystem::KeyState& key_state, float elapsed) {
		if (key_state.pressed) {
			movement_component_.MoveRight();
		}
	});

	input_system_.Register(SDLK_SPACE, [this](InputSystem::KeyState& key_state, float elapsed) {
		if (key_state.pressed) {
			movement_component_.Jump();
			key_state.pressed = false;
		} else if (key_state.released) {
			movement_component_.ReleaseJump();
			key_state.released = false;
		}
	});

	input_system_.Register(SDLK_j, [this, room](InputSystem::KeyState& key_state, float elapsed) {
		if (key_state.pressed) {
			Attack(room);
		}
	});

	animation_controller_.PlayAnimation(AnimationState::STILL, 0.5f, true);
}

bool Player::OnKeyEvent(SDL_Event const &evt)
{
	return input_system_.OnKeyEvent(evt);
}

void Player::Update(float elapsed, const std::vector<Collider*>& colliders_to_consider)
{
	input_system_.Update(elapsed);
	movement_component_.Update(elapsed, colliders_to_consider);
	animation_controller_.Update(elapsed);
	UpdateState();
}

void Player::Draw(DrawSprites& draw) const
{
	animation_controller_.Draw(draw);
}

void Player::SetPosition(const glm::vec2 &pos) {
	transform_.position_ = pos;
}

void Player::Reset() {
	SwitchState(State::MOVING);
	SetPosition(glm::vec2(20.0f, 113.0f));
	hp_ = 100;
}

void Player::TakeDamage(int attack)
{
	take_damage_guard_(kStiffnessTime, [&](){
		std::cout << "Player take damage!" << std::endl;

		int damage = 1;

		if (attack > defense_) {
			damage = attack - defense_;
		}

		hp_ -= damage;

		if (hp_ <= 0) {
			SwitchState(State::DYING);
		} else {
			SwitchState(State::TAKING_DAMAGE);
		}
	});
}

void Player::Attack(Room* room)
{
	attack_guard_(kAttackCooldown, [&](){
		glm::vec2 initial_pos = transform_.position_ + glm::vec2(20.0f, 0.0f) * transform_.scale_;
		glm::vec2 velocity = glm::vec2(200.0f, 0.0f) * transform_.scale_;
		room->AddPlayerAOE(new AOE(glm::vec4(0.0f, 0.0f, 55.0f, 66.0f), &sprites->lookup("fire_1"), velocity, 3.0f, attack_, initial_pos, nullptr));
		SwitchState(State::ATTACKING);

	});
}

void Player::LoadConfig(const std::string& config_file_path)
{
	std::ifstream f(config_file_path);

	if (!f.is_open()) {
		throw std::runtime_error("Fail to load " + config_file_path);
	}

	std::string animation_name;
	int sprite_num;

	while (f >> animation_name >> sprite_num) {
		auto it = kAnimationNameStateMap.find(animation_name);
		if (it == kAnimationNameStateMap.end()) {
			std::cout << "No state correspoonds to " << animation_name << std::endl;
			continue;
		}
		AnimationController<Player::AnimationState>::LoadAnimation((*it).second,
		 "player_" + animation_name, sprite_num);
	}

	// TODO populate prototype
}

// TODO migrate code to state pattern
void Player::UpdateState()
{
	switch (state_)
	{
	case State::MOVING:
		switch (movement_component_.GetState())
		{
			case MovementComponent::State::STILL:
				animation_controller_.PlayAnimation(AnimationState::STILL, 0.5f, true, false);
				break;
			case MovementComponent::State::MOVING:
				animation_controller_.PlayAnimation(AnimationState::WALK, 0.1f, true, false);
				break;
			case MovementComponent::State::JUMPING:
				animation_controller_.PlayAnimation(AnimationState::JUMP, 0.1f, false, false);
				break;
			case MovementComponent::State::FALLING:
				animation_controller_.PlayAnimation(AnimationState::FALL, 0.1f, false, false);
				break;
			default:
				break;
		}
		break;
	case State::ATTACKING:
		break;
	case State::TAKING_DAMAGE:
		break;
	case State::DYING:
		break;
	default:
		throw std::runtime_error("Unknown state: " + std::to_string(static_cast<uint32_t>(state_)));
		break;
	}
}

void Player::EnterState(State state)
{
	switch (state)
	{
	case State::MOVING:
		break;
	case State::ATTACKING:
		animation_controller_.PlayAnimation(AnimationState::ATTACK, 0.1f, false, true);
		TimerManager::Instance().AddTimer(animation_controller_.GetLength(), [&]() {
			if (state_ == State::ATTACKING) {
				SwitchState(State::MOVING);
			}
		});
		break;
	case State::TAKING_DAMAGE:
		animation_controller_.PlayAnimation(AnimationState::HURT, 0.1f, false, true);
		TimerManager::Instance().AddTimer(animation_controller_.GetLength(), [&]() {
			if (state_ == State::TAKING_DAMAGE) {
				SwitchState(State::MOVING);
			}
		});
		break;
	case State::DYING:
		animation_controller_.PlayAnimation(AnimationState::DEATH, 0.1f, false, true);
		TimerManager::Instance().AddTimer(animation_controller_.GetLength(), [&]() {
			Reset();
		});
		break;
	default:
		throw std::runtime_error("Unknown state: " + std::to_string(static_cast<uint32_t>(state)));
		break;
	}
}

void Player::ExitState(State state)
{

}

void Player::SwitchState(State state)
{
	ExitState(state);
	state_ = state;
	EnterState(state);
}
