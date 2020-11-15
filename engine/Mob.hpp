#pragma once

#include "Entity.hpp"
#include "Attack.hpp"
#include "../gamebase/Room.hpp"
#include "TimerGuard.hpp"

#include <algorithm>
#include <vector>

class Mob : public Entity {
public:
	enum class AnimationState : uint8_t {
		STILL = 0,
		WALK,
		JUMP,
		FALL,
		HURT,
		DEATH
	};

	enum class State : uint8_t {
		MOVING = 0,
		ATTACKING,
		TAKING_DAMAGE,
		DYING
	};

	virtual void OnDie() { Destroy(); }

	void TakeDamage(int attack);
	void PerformAttack(Room& room, Attack& attack);

	int GetHp() const { return hp_; }

	virtual void DrawImpl(DrawSprites& draw) {
		animation_controller_.Draw(draw);
	}
	
	static const std::unordered_map<std::string, AnimationState> kAnimationNameStateMap;

protected:
	virtual int GetAttackPoint() { return attack_; }
	virtual int GetDamagePoint(int attack) { return std::max(1, attack - defense_); }

	// @note nullptr will be created for the entry if the animation for the state doesn't exist.
	virtual Animation* GetAnimation(AnimationState state) { return kStateAnimationMap[state]; }
	virtual void PutAnimation(AnimationState state, Animation* animation) { kStateAnimationMap[state] = animation; }

	Mob(const glm::vec4& bounding_box, Transform2D* transform) :
	Entity(bounding_box, transform),
	animation_controller_(&transform_)
	{}

	State state_ { State::MOVING };
	int hp_;
	int attack_;
	int defense_;
	float take_damage_cooldown_;
	AnimationController animation_controller_;
	std::unordered_map<AnimationState, Animation*> kStateAnimationMap;

private:
	TimerGuard take_damage_guard_;
};
