#pragma once

#include <engine/Collider.hpp>
#include <engine/AOE.hpp>
#include <Sprite.hpp>
#include <gamebase/Door.hpp>

#include <vector>

class Player;
class Trigger;
class Monster;
class RoomPrototype;

class Room {
	friend class RoomPrototype;
private:
	// std::vector<Door*> doors_ {};
	const Sprite* background_sprite_;
	const RoomPrototype& room_prototype_;
	Transform2D camera_;

	std::vector<Monster*> monsters_ {};
	std::vector<AOE*> player_AOEs_ {};
	std::vector<AOE*> monster_AOEs_ {};
	
	// layout of this single room
	std::vector<Collider*> platforms_ {};

	std::vector<Trigger*> triggers_ {};

	void Initialize();
	void CleanUp();

	Room(const RoomPrototype& room_prototype);

public:
	void Update(float elapsed, Player* player);
	void Draw(DrawSprites& draw_sprite);
	void AddMonster(Monster* monster) { monsters_.push_back(monster); }
	void AddPlayerAOE(AOE* aoe) { player_AOEs_.push_back(aoe); }
	void AddMonsterAOE(AOE* aoe) { monster_AOEs_.push_back(aoe); }
	void AddTrigger(Trigger* trigger) { triggers_.push_back(trigger); }

	// TODO Room switch
	void OnEnter(Player* player);
	void OnLeave();

	~Room();
};