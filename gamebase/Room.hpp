#pragma once

#include <vector>
#include <unordered_map>
#include "../engine/Collider.hpp"
#include "Monster.hpp"
#include "Door.hpp"

class Room {
private:
	// 	// other rooms that connect with this room
	// 	std::unordered_map<glm::vec2, Door> doors;
	//
	// 	// Monsters in this room
	// 	std::vector<Monster> monsters;
	// 	std::vector<AOE> AOEs;

	// void updateMonsters(float elapsed);
	// void updateAOEs(float elapsed);

public:
	// layout of this single room
	std::vector<Collider*> colliders;

	// void update(float elapsed);
	Room(const std::string& platform_file);
	~Room();
};