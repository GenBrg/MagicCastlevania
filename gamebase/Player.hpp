#pragma once

#include <engine/MovementComponent.hpp>
#include <engine/Transform2D.hpp>
#include <engine/InputSystem.hpp>
#include <engine/TimerGuard.hpp>
#include <engine/Animation.hpp>
#include <engine/Mob.hpp>
#include <engine/Attack.hpp>
#include <gamebase/Buff.hpp>
#include <gamebase/Inventory.hpp>
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
	virtual void UpdateImpl(float elapsed) override;
	virtual void OnDie() override;
	virtual Animation* GetAnimation(AnimationState state) override;

	void UpdatePhysics(float elapsed, const std::vector<Collider*>& colliders_to_consider);
	void StopMovement() { movement_component_.StopMovement(); }
	void SetPosition(const glm::vec2& pos);
	void Reset();
	void AddBuff(const Buff& buff) { buff_ = buff; }
	void ClearBuff();
	void AddHp(int hp);
	void AddMp(int mp);
	void AddExp(int exp);
	void AddCoin(int coin);
	int GetLevel() {return cur_level_;}
    int GetCurLevelExp() {return exp_;}
    int GetCurLevelMaxExp() {
		if (GetLevel() < (int)level_exps_.size()) {
			return level_exps_[GetLevel()];
		}
		else {
			return 10000;
		}
	}
	int GetCoin() {return coin_;}

	// Inventory
	bool PickupItem(ItemPrototype* item);
	void UseItem(size_t slot_num);
	void UnequipItem(size_t slot_num);
	void DropItem(size_t slot_num);
	void DropEquipment(size_t slot_num);
	ItemPrototype* GetItem(size_t slot_num);
	EquipmentPrototype* GetEquipment(size_t slot_num);
	bool IsInventoryFull() const { return inventory_.IsFull(); }
	
	static Player* Create(Room** room, const std::string& player_config_file);
	std::vector<Attack> GetAttackInfo() const;

	virtual int GetAttackPoint() override;
	virtual int GetDefense() override;
	int GetMaxHP() { return max_hp_; }
	virtual void OnTakeDamage() override;

protected:
	virtual void DrawImpl(DrawSprites& draw) override;

private:
	std::unordered_map<Mob::AnimationState, Animation*> animations_;
	MovementComponent movement_component_;
	Room** room_;

	int max_hp_ { 100 };
	int mp_ { 100 };
	int max_mp_ { 100 };
    int exp_ { 0 };
    int cur_level_ {0};
    int coin_ { 0 };
	std::vector<int> level_exps_;
	std::vector<Attack> skills_;
	Buff buff_;
	float shining_duration_ { 0.0f };
	bool is_shining_ { false };

	Inventory inventory_;

	Player(const Player& player);
	Player(Room** room, const glm::vec4 bounding_box);

	void LevelUp();
	void Shine(float duration) { shining_duration_ = duration; }
};
