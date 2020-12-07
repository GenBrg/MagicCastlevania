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
	State GetState() const { return state_; }
	void SetState(State state) { state_ = state; }

	virtual void DrawImpl(DrawSprites& draw) override {
		animation_controller_.Draw(draw);
	}
	virtual int GetAttackPoint() { return attack_; }
	virtual int GetDefense() { return defense_; }
	virtual int GetDamagePoint(int attack) { return std::max(1, attack - GetDefense()); }
	virtual bool IsDestroyed() const override;
	
	static const std::unordered_map<std::string, AnimationState> kAnimationNameStateMap;

protected:
	// @note nullptr will be created for the entry if the animation for the state doesn't exist.
	virtual Animation* GetAnimation(AnimationState state) = 0;

	Mob(const glm::vec4& bounding_box, Transform2D* transform);

	State state_ { State::MOVING };
	int hp_;
	int attack_;
	int defense_;
	float take_damage_cooldown_ { 0.5f };
	bool is_monster_;
	AnimationController animation_controller_;
	int pending_callbacks_ { 0 };

private:
	TimerGuard take_damage_guard_;
};
