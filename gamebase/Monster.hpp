// #pragma once

// #include <vector>
// #include <string>
// #include <SDL2/SDL_events.h>
// #include <Sprite.hpp>
// #include <map>
// #include "Equipment.hpp"
// #include "Collider.hpp"
// #include "Animation.hpp"
// #include "AOE.hpp"

// class Monster {
// private:
// 	// moving pattern of the monster
// 	glm::vec2 position;
// 	glm::vec2 scale;

// 	int healthPoint;
// 	int attackPoint;
// 	int defensePoint;

// 	// regular moving bound (button left x, button left y, top right x, top right y)
// 	glm::vec4 movingBox;

// 	// player enter this trigger box, will attack player
// 	glm::vec4 triggerBox;

// 	// Used for collision detection
// 	Collider collider;

// 	// if currently in battle state
// 	bool battleState;

// 	Animation* animationP;

// public:
// 	// update its position
// 	void update(float elapsed);

// 	bool inBattle();
// 	void onEnterBattleState();

// 	void onAttacked(int attackPoint);
// 	bool defeated();

// 	AOE* generateAOE();

// 	void setCurAnimation(Animation* animation);

// 	// if the death animation completes and this object is ready to free
// 	bool readyToFree();

// };