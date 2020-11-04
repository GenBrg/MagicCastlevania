#include "MovementComponent.hpp"
#include "../Util.hpp"

#include <glm/glm.hpp>

void MovementComponent::MoveLeft()
{
	acceleration_.x = (state_ == State::IN_AIR) ? -horizontal_ground_accelaration_ : -horizontal_air_accelaration_;
}

void MovementComponent::MoveRight()
{
	acceleration_.x = (state_ == State::IN_AIR) ? horizontal_ground_accelaration_ : horizontal_air_accelaration_;
}

void MovementComponent::Jump()
{
	if (state_ != State::IN_AIR) {
		state_ = State::IN_AIR;

		acceleration_.y = kGravity;
		velocity_.y = initial_jump_speed_;
	}
}

void MovementComponent::Update(float elapsed, const std::vector<Collider*>& colliders_to_consider)
{
	// Calculate speed
	glm::vec2 original_velocity = velocity_;
	
	// Calculate acceleration
	// Horizontal
	if (acceleration_.x == 0.0f && state_ != State::IN_AIR) {
		// Fraction
		if (velocity_.x != 0.0f) {
			if (velocity_.x > 0) {
				velocity_.x -= ground_fraction_ * elapsed;
				if (velocity_.x < 0.0f) {
					velocity_.x = 0.0f;
				}
			} else {
				velocity_.x += ground_fraction_ * elapsed;
				if (velocity_.x > 0.0f) {
					velocity_.x = 0.0f;
				}
			}
		}
	} else {
		velocity_.x += acceleration_.x * elapsed;
	}

	acceleration_.x = 0.0f;

	velocity_.y += acceleration_.y * elapsed;

	velocity_.x = glm::clamp(velocity_.x, -max_horizontal_speed_, max_horizontal_speed_);
	velocity_.y = glm::clamp(velocity_.y, -max_vertical_speed_, max_vertical_speed_);
	
	glm::vec2 delta_position = (original_velocity + velocity_) * elapsed / 2.0f;

	// TODO sort colliders in proximity
	for (Collider* other_collider : colliders_to_consider) {
		glm::vec2 contact_point;
		glm::vec2 contact_normal;
		float time;

		if (collider_.DynamicCollisionQuery(*other_collider, delta_position, contact_point, contact_normal, time)) {
			// collision resolution routine

			if (contact_normal.x != 0.0f) {
				velocity_.x = 0.0f;
			} else {
				velocity_.y = 0.0f;
			}

			delta_position += contact_normal * (1 - time) * glm::vec2(std::abs(delta_position.x), std::abs(delta_position.y));
		}
	}
	
	transform_.position_ += delta_position;

	if (transform_.position_.y < 0.0f) {
		transform_.position_.y = 0.0f;
		acceleration_.y = 0.0f;
		state_ = (velocity_.x == 0.0f) ? State::STILL : State::ON_GROUND;
	}
}