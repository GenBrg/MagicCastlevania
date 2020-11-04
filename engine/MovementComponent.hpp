#pragma once

#include "Transform2D.hpp"
#include "Collider.hpp"
#include "../Util.hpp"

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
		ON_GROUND,
		IN_AIR
	};

	MovementComponent(const glm::vec4& box, Transform2D& transform) : 
	transform_(transform),
	collider_(box, &transform)
	{ acceleration_.y = kGravity; }

	// Gameplay
	void MoveLeft();  
	void MoveRight();
	void Jump();
	void Update(float elapsed, const std::vector<Collider*>& colliders_to_consider);

	// Configure
	void SetMaxGroundSpeed(float max_ground_speed) { max_horizontal_speed_ = max_ground_speed; }
	void SetMaxVerticalSpeed(float max_vertical_speed) { max_vertical_speed_ = max_vertical_speed; }
	void SetInitialJumpSpeed(float initial_jump_speed) { initial_jump_speed_ = initial_jump_speed; }
	void SetGroundFraction(float ground_fraction) { ground_fraction_ = ground_fraction; }
	void SetAirFraction(float air_fraction) { air_fraction_ = air_fraction; }
	void SetHorizontalGroundAcceleration(float horizontal_ground_accelaration) { horizontal_ground_accelaration_ = horizontal_ground_accelaration; }
	void SetAirAcceleration(float horizontal_air_accelaration) { horizontal_air_accelaration_ = horizontal_air_accelaration; }

private:
	// Runtime variables
	Transform2D& transform_;
	Collider collider_;
	State state_ { State::STILL };
	glm::vec2 velocity_ { 0.0f };
	glm::vec2 acceleration_ { 0.0f };

	bool move_left_ { false };
	bool move_right_ { false };

	// Constraint configurations
	float max_horizontal_speed_ { 100.0f };
	float max_vertical_speed_ { 1000.0f };
	float initial_jump_speed_ { 100.0f };
	float ground_fraction_ { 200.0f };
	float air_fraction_ { 100.0f };
	float horizontal_ground_accelaration_ { 50.0f };
	float horizontal_air_accelaration_ { 50.0f };
};