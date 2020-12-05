#pragma once

#include <engine/Trigger.hpp>
#include <engine/Transform2D.hpp>
#include <DrawSprites.hpp>
#include <Util.hpp>

class Monster;

class IMonsterAI {
public:
	virtual void Update(float elapsed) = 0;
	virtual ~IMonsterAI() {}
	
	static IMonsterAI* GetMonsterAI(Monster* monster, const json& j);
};

class BasicMovementMonsterAI : public IMonsterAI {
public:
	virtual void Update(float elapsed);
	BasicMovementMonsterAI(const json& j, Monster* monster);

private:
	// Config

	// Runtime
	Transform2D& transform_;
	Monster& monster_;
	float attack_cooldown_;
};

class BouncingMonsterAI : public IMonsterAI {
public:
	virtual void Update(float elapsed);
	BouncingMonsterAI(const json& j, Monster* monster);

private:
	// Config

	// Runtime
	Transform2D& transform_;
	Monster& monster_;

	glm::vec2 speed_vec_;
	float attack_cooldown_;
	float bounce_cooldown_;

	void GenerateSpeedVec();
};

class FollowAndAttackMonsterAI : public IMonsterAI {
public:
	virtual void Update(float elapsed);
	virtual ~FollowAndAttackMonsterAI();
	FollowAndAttackMonsterAI(const json& j, Monster* monster);
	void DebugDrawBoxes(DrawSprites& draw);

private:
	// Config

	// Runtime
	Transform2D& transform_;
	Monster& monster_;

	Trigger* detection_trigger_;
	Trigger* attack_trigger_;
	float attack_cooldown_;

	bool should_move_to_player_ { false };
	bool should_attack_ { false };
};

class RandomWalkingMonsterAI : public IMonsterAI {
public:
	virtual void Update(float elapsed);
	RandomWalkingMonsterAI(const json& j, Monster* monster);

private:
	// Config

	// Runtime
	Transform2D& transform_;
	Monster& monster_;

	glm::vec2 target_pos_;
	float attack_cooldown_;

	void GenerateTargetPos();
};