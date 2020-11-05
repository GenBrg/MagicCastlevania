#pragma once

#include "../engine/Collider.hpp"
#include "../engine/AOE.hpp"
#include "Monster.hpp"
#include "Door.hpp"

#include <vector>
#include <unordered_map>

class Player;
class Room {
private:
	// 	// other rooms that connect with this room
	// 	std::unordered_map<glm::vec2, Door> doors;

	std::vector<Monster*> monsters_ {};
	std::vector<AOE*> player_AOEs_ {};
	std::vector<AOE*> monster_AOEs_ {};
	
	// layout of this single room
	std::vector<Collider*> platforms_ {};

public:
	void Update(float elapsed, Player* player);
	void Draw(DrawSprites& draw_sprite);
	void AddPlayerAOE(AOE* aoe) { player_AOEs_.push_back(aoe); }
	void AddMonsterAOE(AOE* aoe) { monster_AOEs_.push_back(aoe); }

	Room(const std::string& platform_file);
	~Room();
};