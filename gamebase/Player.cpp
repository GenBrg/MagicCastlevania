#include "Player.hpp"

#include <Util.hpp>
#include <Load.hpp>
#include <engine/Timer.hpp>
#include <gamebase/Room.hpp>

#include <SDL.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>

Player::Player(Room** room, const glm::vec4 bounding_box) :
Mob(bounding_box, nullptr),
movement_component_(collider_, transform_),
room_(room)
{
	take_damage_cooldown_ = 1.0f;
	is_monster_ = false;

	InputSystem::Instance()->Register(SDLK_a, [this](InputSystem::KeyState& key_state, float elapsed) {
		if (state_ != State::MOVING) {
			return;
		}
		if (key_state.pressed) {
			movement_component_.MoveLeft();
		}
	});

	InputSystem::Instance()->Register(SDLK_d, [this](InputSystem::KeyState& key_state, float elapsed) {
		if (state_ != State::MOVING) {
			return;
		}
		if (key_state.pressed) {
			movement_component_.MoveRight();
		}
	});

	InputSystem::Instance()->Register(SDLK_SPACE, [this](InputSystem::KeyState& key_state, float elapsed) {
		if (state_ != State::MOVING) {
			return;
		}
		if (key_state.pressed) {
			movement_component_.Jump();
			key_state.pressed = false;
		} else if (key_state.released) {
			movement_component_.ReleaseJump();
			key_state.released = false;
		}
	});

	InputSystem::Instance()->Register(SDLK_j, [this, room](InputSystem::KeyState& key_state, float elapsed) {
		if (state_ != State::MOVING) {
			return;
		}
		if (key_state.pressed) {
			PerformAttack(**room, skills_[0]);
		}
	});
}

void Player::OnDie()
{
	state_ = State::DYING;
	animation_controller_.PlayAnimation(GetAnimation(AnimationState::DEATH), false);
	TimerManager::Instance().AddTimer(GetAnimation(AnimationState::DEATH)->GetLength(), [&](){
		Reset();
	});
}

void Player::UpdateImpl(float elapsed)
{
	animation_controller_.Update(elapsed);
	if (state_ == Mob::State::MOVING) {
		switch (movement_component_.GetState())
		{
			case MovementComponent::State::STILL:
				animation_controller_.PlayAnimation(GetAnimation(AnimationState::STILL), true, false);
				break; 
			case MovementComponent::State::MOVING:
				animation_controller_.PlayAnimation(GetAnimation(AnimationState::WALK), true, false);
				break;
			case MovementComponent::State::JUMPING:
				animation_controller_.PlayAnimation(GetAnimation(AnimationState::JUMP), false, false);
				break;
			case MovementComponent::State::FALLING:
				animation_controller_.PlayAnimation(GetAnimation(AnimationState::FALL), false, false);
				break;
			default:
				break;
		}
	}

	buffs_.erase(std::remove_if(buffs_.begin(), buffs_.end(), [&](Buff& buff){
		if (buff.Update(elapsed)) {
			return true;
		}
		return false;
	}),buffs_.end());
}

void Player::UpdatePhysics(float elapsed, const std::vector<Collider*>& colliders_to_consider)
{
	movement_component_.Update(elapsed, colliders_to_consider);
}

void Player::SetPosition(const glm::vec2 &pos) {
	transform_.position_ = pos;
}

void Player::Reset() {
	state_ = State::MOVING;
	SetPosition((*room_)->GetDoor(0)->GetPosition());
	hp_ = max_hp_;
}

Player* Player::Create(Room** room, const std::string& player_config_file)
{
	std::ifstream f(player_config_file);

	if (!f.is_open()) {
		throw std::runtime_error("Can not load player config file: " + player_config_file);
	}

	json j;
	f >> j;

	Player* player = new Player(room, j.at("bounding_box").get<glm::vec4>());
	player->hp_ = j.at("hp").get<int>();
	player->attack_ = j.at("attack").get<int>();
	player->defense_ = j.at("defense").get<int>();
	player->take_damage_cooldown_ = j.at("invulnerable_time").get<float>();

	auto& movement_component_json = j.at("movement");
	player->movement_component_.SetMaxGroundSpeed(movement_component_json.at("max_horizontal_speed").get<float>());
	player->movement_component_.SetMaxVerticalSpeed(movement_component_json.at("max_vertical_speed").get<float>());
	player->movement_component_.SetInitialJumpSpeed(movement_component_json.at("initial_jump_speed").get<float>());
	player->movement_component_.SetMaxSpeedAfterJumpRelease(movement_component_json.at("max_speed_after_jump_release").get<float>());
	player->movement_component_.SetGroundFraction(movement_component_json.at("ground_fraction").get<float>());
	player->movement_component_.SetAirFraction(movement_component_json.at("air_fraction").get<float>());
	player->movement_component_.SetHorizontalGroundAcceleration(movement_component_json.at("horizontal_ground_accelaration").get<float>());
	player->movement_component_.SetHorizontalAirAcceleration(movement_component_json.at("horizontal_air_accelaration").get<float>());

	for (const auto& attack_json : j.at("skills")) {
		player->skills_.push_back(attack_json.get<Attack>());
	}

	// Load Animations
	for (const auto& [animation_name, animation_state_name] : Mob::kAnimationNameStateMap) {
		player->animations_[animation_state_name] = Animation::GetAnimation("player_" + animation_name);
	}

	// Load level config
	for (const auto& level_exp : j.at("level_exps")) {
	    player->level_exps_.push_back(level_exp);
	}

	return player;
}

Animation* Player::GetAnimation(AnimationState state)
{
	return animations_[state];
}

std::vector<Attack> Player::GetAttackInfo() const {
    return skills_;
}

int Player::GetAttackPoint()
{
	int attack = attack_;
	for (const Buff& buff : buffs_) {
		attack = buff.ApplyAttack(attack);
	}
	attack = inventory_.ApplyEquipmentAttack(attack);
	return attack;
}

int Player::GetDamagePoint(int attack)
{
	int defense = defense_;
	for (const Buff& buff : buffs_) {
		defense = buff.ApplyDefense(defense);
	}
	defense = inventory_.ApplyEquipmentAttack(defense);
	return Mob::GetDamagePoint(attack);
}

void Player::AddHp(int hp)
{
	hp_ += hp;
	hp_ = std::min(hp_, max_hp_);
}

void Player::AddMp(int mp)
{
	mp_ += mp;
	mp_ = std::min(mp_, max_mp_);
}

void Player::AddExp(int exp)
{
    if(cur_level_ == (int)level_exps_.size() - 1 && exp_ == level_exps_[cur_level_]) {
        return;
    }

    exp_ += exp;
    while (cur_level_ <= (int)level_exps_.size() - 1 && exp_ >= level_exps_[cur_level_]) {
        exp_ -= level_exps_[cur_level_];
        cur_level_++;
    }

    if (cur_level_ == (int)level_exps_.size()) {
        // already max level, truncate exp to not overflow
        cur_level_--;
        exp_ =  level_exps_[cur_level_];
    }
}

void Player::AddCoin(int coin) {
    coin_ += coin;
}

bool Player::PickupItem(ItemPrototype* item)
{
	return inventory_.PushItem(item);
}

void Player::UseItem(size_t slot_num)
{
	inventory_.UseItem(this, slot_num);
}

void Player::UnequipItem(size_t slot_num)
{
	inventory_.UnequipItem(slot_num);
}

void Player::DropItem(size_t slot_num)
{
	inventory_.PopItem(slot_num);
}

void Player::DropEquipment(size_t slot_num)
{
	inventory_.PopEquipment(slot_num);
}

ItemPrototype* Player::GetItem(size_t slot_num)
{
	return inventory_.PeekItem(slot_num);
}

EquipmentPrototype* Player::GetEquipment(size_t slot_num)
{
	return inventory_.PeekEquipment(slot_num);
}

