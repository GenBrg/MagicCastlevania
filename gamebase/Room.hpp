#pragma once

#include <vector>
#include "Monster.hpp"
#include "Door.hpp"

class Room {
private:
	// layout of this single room
	std::vector<Collider> colliders;

	// other rooms that connect with this room
	std::unordered_map<glm::vec2, Door> doors;

	// Monsters in this room
	std::vector<Monster> monsters;
	std::vector<AOE> AOEs;

	void updateMonsters(float elapsed);
	void updateAOEs(float elapsed);

public:
	void update(float elapsed);
};