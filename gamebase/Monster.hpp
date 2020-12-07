#pragma once

#include <engine/Mob.hpp>
#include <engine/AOE.hpp>
#include <engine/MovementComponent.hpp>
#include <gamebase/MonsterAI.hpp>
#include <Sprite.hpp>
#include <DrawSprites.hpp>

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

	AOE* collision_aoe_ { nullptr };

	virtual void UpdateImpl(float elapsed);
	virtual void OnDie();
	virtual Animation* GetAnimation(AnimationState state);
	
    int exp_ { 10 };
    int coin_;
	float speed_ { 50.0f };
	std::vector<Attack> skills_;
	Room& room_;
	IMonsterAI* ai_;

	Monster(const MonsterPrototype& monster_prototype, const glm::vec4& bounding_box, int body_attack, Room& room);

public:
	Attack* GetAttack(size_t type) { return &(skills_.at(type)); }
	size_t GetAttackNum() const { return skills_.size(); }
	Room& GetRoom() { return room_; }
	float GetMoveRadius() const { return move_radius_; }
	glm::vec2 GetCentralPos() const { return central_pos_; }
	float GetSpeed() const { return speed_; }
	virtual void Destroy() override;
	virtual void PlayTakeDamageSound() override;

	~Monster();
};
