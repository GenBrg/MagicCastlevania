#pragma once

#include <engine/Collider.hpp>
#include <engine/AOE.hpp>
#include <Sprite.hpp>
#include <gamebase/Door.hpp>
#include <gamebase/ItemPickup.hpp>

#include <vector>
#include "Dialog.hpp"
#include "Shop.hpp"

class Player;
class Trigger;
class Monster;
class RoomPrototype;
class Door;
class DoorKey;

class Room {
	friend class RoomPrototype;
private:
	std::vector<Door*> doors_ {};
	const Sprite* background_sprite_;
	const RoomPrototype& room_prototype_;
	Transform2D camera_;

	std::vector<Monster *> monsters_{};
	std::vector<AOE *> player_AOEs_{};
	std::vector<AOE *> monster_AOEs_{};
	std::vector<ItemPickUp *> items_{};
	std::vector<ItemPickUp *> permanent_items_{};
	DoorKey* door_key_ { nullptr };
	Shop* shop_ {nullptr};

	// layout of this single room
	std::vector<Collider *> platforms_{};

	std::vector<Trigger *> triggers_{};

	std::vector<Dialog *> dialogs_{};

	void Initialize();

	void CleanUp();

	Room(const RoomPrototype &room_prototype);

public:
	Dialog* cur_dialog = nullptr;
	void Update(float elapsed, Player* player, Door** cur_door);
	void Draw(DrawSprites& draw_sprite);
	void AddMonster(Monster* monster) { monsters_.push_back(monster); }
	void AddPlayerAOE(AOE* aoe) { player_AOEs_.push_back(aoe); }
	void AddMonsterAOE(AOE* aoe) { monster_AOEs_.push_back(aoe); }
	void AddTrigger(Trigger* trigger) { triggers_.push_back(trigger); }
	void AddDoor(Door* door) { doors_.push_back(door); }
	void AddItem(ItemPickUp* item) { items_.push_back(item); }
	void AddPermanentItem(ItemPickUp* item) { permanent_items_.push_back(item); }
	void GenerateKey();

	size_t GetDoorNum() const { return doors_.size(); }
	Door* GetDoor(size_t idx) { return doors_[idx]; }

	// TODO Room switch
	void OnEnter(Player *player, Door* door);

	void OnLeave();

	~Room();
};