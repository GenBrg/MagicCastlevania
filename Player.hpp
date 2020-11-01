#pragma once

#include "Sprite.hpp"

#include <glm/glm.hpp>

class Player {
public:
	inline static constexpr float kMaxHorizontalSpeed { 5.0f };
	inline static constexpr float kJumpInitialSpeed { 10.0f };
	inline static constexpr float kFraction { 2.0f };
	inline static constexpr float kHorizontalAccelerationOnGround { 5.0f };
	inline static constexpr float kHorizontalAccelerationInAir { 5.0f };

	enum class MovingState : uint8_t {
		STILL = 0,
		ON_GROUND,
		IN_AIR
	};

	void MoveLeft();
	void MoveRight();
	void Jump();
	void Update(float elapsed);
	void Draw(const DrawSprites& draw) const;

	Player();

private:
	glm::vec2 position_ { 0.0f };
	glm::vec2 velocity_ { 0.0f };
	glm::vec2 acceleration_ { 0.0f };
	Sprite& sprite_;
	MovingState moving_state_ { MovingState::STILL };
};
