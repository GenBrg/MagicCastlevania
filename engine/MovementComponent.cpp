#include "MovementComponent.hpp"

#include <glm/glm.hpp>

#include <iostream>

void MovementComponent::MoveLeft()
{
	acceleration_.x = (velocity_.y != 0.0f) ? -horizontal_ground_accelaration_ : -horizontal_air_accelaration_;
}

void MovementComponent::MoveRight()
{
	acceleration_.x = (velocity_.y != 0.0f) ? horizontal_ground_accelaration_ : horizontal_air_accelaration_;
}

void MovementComponent::Jump()
{
	if (velocity_.y == 0.0f) {
		velocity_.y = initial_jump_speed_;
	}
}

void MovementComponent::Update(float elapsed, const std::vector<Collider*>& colliders_to_consider)
{
	glm::vec2 original_velocity = velocity_;
	
	// Calculate acceleration
	// Horizontal
	if (acceleration_.x == 0.0f) {
		float fraction = (velocity_.y == 0.0f) ? ground_fraction_ : air_fraction_;

		// Fraction
		if (velocity_.x != 0.0f) {
			if (velocity_.x > 0) {
				velocity_.x -= fraction * elapsed;
				if (velocity_.x < 0.0f) {
					velocity_.x = 0.0f;
				}
			} else {
				velocity_.x += fraction * elapsed;
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
}