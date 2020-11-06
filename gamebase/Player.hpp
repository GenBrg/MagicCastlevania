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

#include "../engine/MovementComponent.hpp"
#include "../engine/Transform2D.hpp"
#include "../engine/InputSystem.hpp"
#include "../engine/TimerGuard.hpp"
#include "../engine/Animation.hpp"
#include "../Sprite.hpp"
#include "../DrawSprites.hpp"

#include <glm/glm.hpp>

#include <chrono>
#include <string>

class Room;
class Player {
public:
	inline constexpr static float kStiffnessTime { 2.0f };
	inline constexpr static float kAttackCooldown { 1.0f };

	bool OnKeyEvent(SDL_Event const &evt);
	void Update(float elapsed, const std::vector<Collider*>& colliders_to_consider);
	void Draw(DrawSprites& draw) const;
	void SetPosition(const glm::vec2& pos);

	void TakeDamage(int attack);
	void Attack(Room* room);
	void Reset();
	Collider* GetCollider() { return movement_component_.GetCollider(); }

	Player(Room* room);

	int GetHp() const { return hp_; }

	// TODO Apply prototype pattern
	// static Player* GetCopy();
	static void LoadConfig(const std::string& config_file_path);

private:
	Transform2D transform_;
	InputSystem input_system_;
	MovementComponent movement_component_;
	AnimationController animation_controller_;

	// static Player prototype_;

	// int level_ { 1 };
	int hp_ { 100 };
	// int max_hp_ { 100 };
	// int mp_ { 100 };
	// int max_mp_ { 100 };
	int attack_ { 10 };
	int defense_ { 10 };
	// int exp_ { 0 };
	// int max_exp_ { 100 };
	
	const Sprite& sprite_;

	TimerGuard attack_guard_;
	TimerGuard take_damage_guard_;

	Player(const Player& player);
	// Player() = default;
};
