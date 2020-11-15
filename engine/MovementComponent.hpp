#pragma once

#include "Transform2D.hpp"
#include "Collider.hpp"
#include "../Util.hpp"
#include "TimerGuard.hpp"

#include <glm/glm.hpp>

#include <vector>

/**
 * Class that extracts the common behavior of all moving objects like player character.
 * It collabrates with Collider to control a Transform2D.
 * @author Jiasheng Zhou
 */
class MovementComponent {
public:
	enum class State : uint8_t {
		STILL = 0,
		MOVING,
		JUMPING,
		FALLING
	};

	MovementComponent(Collider& collider, Transform2D& transform) : 
	transform_(transform),
	collider_(collider)
	{ acceleration_.y = kGravity; }

	// Gameplay
	void MoveLeft();  
	void MoveRight();
	void Jump();
	void ReleaseJump();
	void Update(float elapsed, const std::vector<Collider*>& colliders_to_consider);

	// Configure
	void SetMaxGroundSpeed(float max_ground_speed) { max_horizontal_speed_ = max_ground_speed; }
	void SetMaxVerticalSpeed(float max_vertical_speed) { max_vertical_speed_ = max_vertical_speed; }
	void SetInitialJumpSpeed(float initial_jump_speed) { initial_jump_speed_ = initial_jump_speed; }
	void SetGroundFraction(float ground_fraction) { ground_fraction_ = ground_fraction; }
	void SetAirFraction(float air_fraction) { air_fraction_ = air_fraction; }
	void SetHorizontalGroundAcceleration(float horizontal_ground_accelaration) { horizontal_ground_accelaration_ = horizontal_ground_accelaration; }
	void SetHorizontalAirAcceleration(float horizontal_air_accelaration) { horizontal_air_accelaration_ = horizontal_air_accelaration; }
	void SetAirAcceleration(float horizontal_air_accelaration) { horizontal_air_accelaration_ = horizontal_air_accelaration; }
	void SetMaxSpeedAfterJumpRelease(float max_speed_after_jump_release) { max_speed_after_jump_release_ = max_speed_after_jump_release; }

	State GetState() const { return state_; }

private:
	// Runtime variables
	Transform2D& transform_;
	Collider& collider_;
	glm::vec2 velocity_ { 0.0f };
	glm::vec2 acceleration_ { 0.0f };
	State state_ { State::STILL };
	int jump_chance_ { 1 };
	int max_jump_chance_ { 1 };

	// Constraint configurations
	float max_horizontal_speed_ { 200.0f };
	float max_vertical_speed_ { 1000.0f };
	float initial_jump_speed_ { 600.0f };
	float max_speed_after_jump_release_ { 300.0f };
	float ground_fraction_ { 1000.0f };
	float air_fraction_ { 300.0f };
	float horizontal_ground_accelaration_ { 1300.0f };
	float horizontal_air_accelaration_ { 1300.0f };
};