#pragma once

#include <vector>
#include <SDL2/SDL_events.h>
#include "Equipment.hpp"
#include "Collider.hpp"
#include "Animation.hpp"
#include "AOE.hpp"

class Player {
private:
	int healthPoint;
	int attackPoint;
	int defensePoint;
	std::vector<Equipment> equipments;

	// position of the player in the room
	glm::vec2 position;
	glm::vec2 scale;

	// Used for collision detection
	Collider collider;

	Animation animation;
public:
	Player();

	// update player related
	void onKeyEvent(SDL_Event const &evt);
	void update(float elapse);

	void onAttacked(int attackPoint);
	bool defeated();

	AOE* generateAOE();
};