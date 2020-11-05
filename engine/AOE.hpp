// #pragma once

// #include "glm/glm.hpp"
// #include "Animation.hpp"
// #include "Collider.hpp"

// class AOE {
// private:
// 	glm::vec2 position;
// 	glm::vec2 scale;
// 	glm::vec2 velocity;

// 	Animation animation;
// 	Collider collider;
// };

#pragma once

#include "Transform2D.hpp"
#include "Collider.hpp"
#include "Sprite.hpp"
#include "DrawSprites.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <functional>

class AOE {
public:
	using CollisionQuery = std::pair<Collider*, std::function<void()>>;

	/**
	 * @param box Bounding box of the AOE for collision detection.
	 * @param velocity Velocity of the AOE.
	 * @param duration Duration of the AOE, for number less than 0.0f, it will remain forever.
	 * @param attack The power of the AOE.
	 * @param parent_transform The transform the AOE is moving with.
	 */
	AOE(const glm::vec4& box, Sprite* sprite, const glm::vec2& velocity, float duration, int attack, Transform2D* parent_transform = nullptr);

	int GetAttack() const { return attack_; }
	void Update(float elapsed, const std::vector<CollisionQuery>& collision_queries);
	void Draw(DrawSprites& draw_sprite);
	void Destroy() { destroyed_ = true; }

private:
	Transform2D transform_;
	Collider collider_;
	glm::vec2 velocity_;
	float duration_;
	Sprite* sprite_;
	// Animation animation_;
	int attack_;

	bool destroyed_ { false };
};
