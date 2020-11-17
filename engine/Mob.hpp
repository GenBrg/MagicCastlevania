#pragma once

#include <gamebase/Room.hpp>
#include <engine/TimerGuard.hpp>
#include <engine/Animation.hpp>
#include <engine/Entity.hpp>
#include <engine/Attack.hpp>

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
	virtual Animation* GetAnimation(AnimationState state) = 0;

	Mob(const glm::vec4& bounding_box, Transform2D* transform);

	State state_ { State::MOVING };
	int hp_;
	int attack_;
	int defense_;
	float take_damage_cooldown_ { 3.0f };
	bool is_monster_;
	AnimationController animation_controller_;

private:
	TimerGuard take_damage_guard_;
};