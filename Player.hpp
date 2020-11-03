#pragma once

#include "Sprite.hpp"
#include "DrawSprites.hpp"

#include <glm/glm.hpp>

class Player {
public:
	inline static constexpr float kMaxHorizontalSpeed { 100.0f };
	inline static constexpr float kJumpInitialSpeed { 50.0f };
	inline static constexpr float kFraction { 200.0f };
	inline static constexpr float kHorizontalAccelerationOnGround { 50.0f };
	inline static constexpr float kHorizontalAccelerationInAir { 50.0f };

	enum class MovingState : uint8_t {
		STILL = 0,
		ON_GROUND,
		IN_AIR
	};

	void MoveLeft();      
	void MoveRight();
	void Jump();
	void Update(float elapsed);
	void Draw(DrawSprites& draw) const;

	Player();

private:
	glm::vec2 position_ { 0.0f };
	glm::vec2 velocity_ { 0.0f };
	glm::vec2 acceleration_ { 0.0f };
	const Sprite& sprite_;
	MovingState moving_state_ { MovingState::STILL };
};
