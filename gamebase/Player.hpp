// #pragma once

// #include <vector>
// #include <SDL2/SDL_events.h>
// #include "Equipment.hpp"
// #include "Collider.hpp"
// #include "Animation.hpp"
// #include "AOE.hpp"

// class Player {
// private:
// 	int healthPoint;
// 	int attackPoint;
// 	int defensePoint;
// 	std::vector<Equipment> equipments;

// 	// position of the player in the room
// 	glm::vec2 position;
// 	glm::vec2 scale;

// 	// Used for collision detection
// 	Collider collider;

// 	Animation animation;
// public:
// 	Player();

// 	// update player related
// 	void onKeyEvent(SDL_Event const &evt);
// 	void update(float elapse);

// 	void onAttacked(int attackPoint);
// 	bool defeated();

// 	AOE* generateAOE();
// };

#pragma once

#include <engine/MovementComponent.hpp>
#include <engine/Transform2D.hpp>
#include <engine/InputSystem.hpp>
#include <engine/TimerGuard.hpp>
#include <engine/Animation.hpp>
#include <engine/Mob.hpp>
#include <engine/Attack.hpp>
#include <Sprite.hpp>
#include <DrawSprites.hpp>

#include <glm/glm.hpp>

#include <chrono>
#include <string>
#include <unordered_map>
#include <string>

class Room;
class Player : public Mob {
public:
	virtual void UpdateImpl(float elapsed);
	virtual void OnDie();
	virtual Animation* GetAnimation(AnimationState state);

	void UpdatePhysics(float elapsed, const std::vector<Collider*>& colliders_to_consider);
	void SetPosition(const glm::vec2& pos);
	void Reset();

	static Player* Create(Room** room, const std::string& player_config_file);

private:
	std::unordered_map<Mob::AnimationState, Animation*> animations_;
	MovementComponent movement_component_;
	Room** room_;

	// int level_ { 1 };
	int max_hp_ { 100 };
	// int mp_ { 100 };
	// int max_mp_ { 100 };
	// int exp_ { 0 };
	// int max_exp_ { 100 };
	std::vector<Attack> skills_;

	Player(const Player& player);
	Player(Room** room, const glm::vec4 bounding_box);
};
