#pragma once

#include <vector>
#include <string>
#include "../Sprite.hpp"
#include "../engine/MovementComponent.hpp"
#include "../DrawSprites.hpp"
#include <map>



class Monster {
private:
	Transform2D transform_;
	MovementComponent movement_component_;
	const Sprite& sprite_;

	// tmp
	float move_radius_;
	glm::vec2 central_pos_;
	int mov_direction_;
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

public:
	Monster(const glm::vec2& pos, float move_radius, std::string monster_key);
	// update its position
	void Update(float elapsed, const std::vector<Collider*>& colliders_to_consider);
	void Draw(DrawSprites& draw) const;


// 	bool inBattle();
// 	void onEnterBattleState();

// 	void onAttacked(int attackPoint);
// 	bool defeated();

// 	AOE* generateAOE();

// 	void setCurAnimation(Animation* animation);

// 	// if the death animation completes and this object is ready to free
// 	bool readyToFree();

};