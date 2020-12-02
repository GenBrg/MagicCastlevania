#pragma once

#include <engine/MovementComponent.hpp>
#include <engine/Transform2D.hpp>
#include <engine/InputSystem.hpp>
#include <engine/TimerGuard.hpp>
#include <engine/Animation.hpp>
#include <engine/Mob.hpp>
#include <engine/Attack.hpp>
#include <gamebase/Buff.hpp>
#include <Sprite.hpp>
#include <DrawSprites.hpp>

#include <glm/glm.hpp>

#include <chrono>
#include <string>
#include <unordered_map>
#include <string>
#include <vector>

class Room;
class Player : public Mob {
public:
	virtual void UpdateImpl(float elapsed);
	virtual void OnDie();
	virtual Animation* GetAnimation(AnimationState state);

	void UpdatePhysics(float elapsed, const std::vector<Collider*>& colliders_to_consider);
	void SetPosition(const glm::vec2& pos);
	void Reset();
	void AddBuff(const Buff& buff) { buffs_.push_back(buff); }
	void AddHp(int hp);
	void AddMp(int mp);
	void AddExp(int exp);
	
	static Player* Create(Room** room, const std::string& player_config_file);
	std::vector<Attack> GetAttackInfo() const;

	virtual int GetAttackPoint() override;
	virtual int GetDamagePoint(int attack) override;

private:
	std::unordered_map<Mob::AnimationState, Animation*> animations_;
	MovementComponent movement_component_;
	Room** room_;

	int level_ { 1 };
	int max_hp_ { 100 };
	int mp_ { 100 };
	int max_mp_ { 100 };
	int exp_ { 0 };
	int max_exp_ { 100 };
	std::vector<Attack> skills_;
	std::vector<Buff> buffs_;

	Player(const Player& player);
	Player(Room** room, const glm::vec4 bounding_box);

	void LevelUp();
};
