 #include "Player.hpp"
#include "Util.hpp"

Player::Player() :
sprite_(sprites->lookup("dunes-ship")),
position_()
{
}

void Player::MoveLeft()
{
	acceleration_.x = (moving_state_ == MovingState::IN_AIR) ? -kHorizontalAccelerationOnGround : -kHorizontalAccelerationInAir;
}

void Player::MoveRight()
{
	acceleration_.x = (moving_state_ == MovingState::IN_AIR) ? kHorizontalAccelerationOnGround : kHorizontalAccelerationInAir;
}

void Player::Jump()
{
	if (moving_state_ != MovingState::IN_AIR) {
		moving_state_ = MovingState::IN_AIR;

		acceleration_.y = kGravity;
		velocity_.y = kJumpInitialSpeed;
	}
}

void Player::Update(float elapsed)
{
	// Calculate speed
	glm::vec2 original_velocity = velocity_;
	
	// Calculate acceleration
	// Horizontal
	if (acceleration_.x == 0.0f && moving_state_ != MovingState::IN_AIR) {
		// Fraction
		if (velocity_.x != 0.0f) {
			if (velocity_.x > 0) {
				velocity_.x -= kFraction * elapsed;
				if (velocity_.x < 0.0f) {
					velocity_.x = 0.0f;
				}
			} else {
				velocity_.x += kFraction * elapsed;
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

	velocity_.x = glm::clamp(velocity_.x, -kMaxHorizontalSpeed, kMaxHorizontalSpeed);
	
	position_ += (original_velocity + velocity_) * elapsed / 2.0f;

	if (position_.y < 0.0f) {
		position_.y = 0.0f;
		acceleration_.y = 0.0f;
		moving_state_ = (velocity_.x == 0.0f) ? MovingState::STILL : MovingState::ON_GROUND;
	}
}

void Player::Draw(DrawSprites& draw) const
{
	draw.draw(sprite_, position_);
}
