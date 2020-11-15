#pragma once

#include "../engine/Mob.hpp"
#include "../engine/AOE.hpp"
#include "../Sprite.hpp"
#include "../engine/MovementComponent.hpp"
#include "../DrawSprites.hpp"

#include <map>
#include <vector>
#include <string>

class Room;
class MonsterPrototype;

class Monster : public Mob {
	friend class MonsterPrototype;
private:
	const MonsterPrototype& monster_prototype_;
	float move_radius_;
	glm::vec2 central_pos_;
	int mov_direction_ { 1 };

	bool destroyed_ { false };
	AOE* collision_aoe_ { nullptr };

	virtual void UpdateImpl(float elapsed);
	virtual void OnDie();

	int hp_ { 20 };
	// int max_hp_ { 100 };
	int attack_ { 50 };
	int defense_ { 3 };
	int exp_ { 10 };
	float speed_ { 50.0f };

	Monster(const MonsterPrototype& monster_prototype, const glm::vec4& bounding_box, int body_attack, Room& room);

public:
	// static Monster* CreateTestMonster(const glm::vec4& bounding_box, const glm::vec2& initial_pos, float move_radius, std::string monster_key, Room* room, int body_attack);
};