#include "MovementComponent.hpp"

#include <glm/glm.hpp>
#include <engine/Random.hpp>

#include <iostream>
#include <stdexcept>
#include <engine/Random.hpp>

void MovementComponent::MoveLeft()
{
	acceleration_.x = (state_ == State::STILL || state_ == State::MOVING) ? -horizontal_ground_accelaration_ : -horizontal_air_accelaration_;
	transform_.scale_ = glm::vec2(-1.0f, 1.0f);
}

void MovementComponent::MoveRight()
{
	acceleration_.x = (state_ == State::STILL || state_ == State::MOVING) ? horizontal_ground_accelaration_ : horizontal_air_accelaration_;
	transform_.scale_ = glm::vec2(1.0f, 1.0f);
}

void MovementComponent::Jump()
{
	if (jump_chance_ > 0) {
		--jump_chance_;
		velocity_.y = initial_jump_speed_;
        int sound_idx = static_cast<int>(Random::Instance()->Generate() * 3) + 1;
        Sound::play(*sound_samples.at("jump_" + std::to_string(sound_idx)));
	}
}

void MovementComponent::ReleaseJump() 
{
	if (state_ == State::JUMPING) {
		velocity_.y = glm::min(velocity_.y, max_speed_after_jump_release_);
	}
}

void MovementComponent::Update(float elapsed, const std::vector<Collider*>& colliders_to_consider)
{
	glm::vec2 original_velocity = velocity_;

	bool has_external_force = acceleration_.x != 0.0f;
	
	// Calculate acceleration
	// Horizontal
	float fraction = 0.0f;
	if (state_ != State::STILL) {
		fraction = (state_ == State::MOVING) ? ground_fraction_ : air_fraction_;
	}

	// Fraction
	if (velocity_.x > 0) {
		acceleration_.x -= fraction;
	} else if (velocity_.x < 0) {
		acceleration_.x += fraction;
	}
	
	velocity_.x += acceleration_.x * elapsed;
	acceleration_.x = 0.0f;

	// Avoid fraction from causing movement in opposite direction
	if (!has_external_force) {
		if (original_velocity.x * velocity_.x <= 0.0f) {
			velocity_.x = 0.0f;
		}
	}

	velocity_.y += acceleration_.y * elapsed;

	velocity_.x = glm::clamp(velocity_.x, -max_horizontal_speed_, max_horizontal_speed_);
	velocity_.y = glm::clamp(velocity_.y, -max_vertical_speed_, max_vertical_speed_);
	
	glm::vec2 delta_position = (original_velocity + velocity_) * elapsed / 2.0f;

	bool hit_ground = false;

	// TODO sort colliders in proximity
	for (Collider* other_collider : colliders_to_consider) {
		glm::vec2 contact_point;
		glm::vec2 contact_normal;
		float time;

		if (collider_.DynamicCollisionQuery(*other_collider, delta_position, contact_point, contact_normal, time)) {
			// collision resolution routine
			if (time < 0.0f) {
				std::cout << time << std::endl;
			}

			if (contact_normal.x != 0.0f) {
				velocity_.x = 0.0f;
			} else {
				velocity_.y = 0.0f;
			}

			delta_position += contact_normal * (1 - time) * glm::vec2(std::abs(delta_position.x), std::abs(delta_position.y));

			if (contact_normal.y > 0.0f) {
				hit_ground = true;
			}
		}
	}
	
	transform_.position_ += delta_position;

	switch (state_) {
		case State::STILL:
			if (velocity_.y > 0.0f) {
				state_ = State::JUMPING;
			} else if (velocity_.y < 0.0f) {
				--jump_chance_;
				state_ = State::FALLING;
			} else if (velocity_.x != 0.0f) {
				state_ = State::MOVING;
			}
		break;
		case State::JUMPING:
			if (velocity_.y < 0.0f) {
				state_ = State::FALLING;
			}
		break;
		case State::FALLING:
			if (hit_ground) {
				jump_chance_ = max_jump_chance_;
				state_ = (velocity_.x == 0.0f) ? State::STILL : State::MOVING;
			}
		break;
		case State::MOVING:
			if (velocity_.y > 0.0f) {
				state_ = State::JUMPING;
			} else if (velocity_.y < 0.0f) {
				--jump_chance_;
				state_ = State::FALLING;
			} else if (velocity_.x == 0.0f) {
				state_ = State::STILL;
			}
		break;
		default:
		throw std::runtime_error("Unknown MovementComponent::State!");
	}
}