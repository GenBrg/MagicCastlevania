#pragma once

#include "Transform2D.hpp"
#include "Collider.hpp"
#include "Animation.hpp"
#include "../Sprite.hpp"
#include "../DrawSprites.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <functional>

class Room;

class AOE {
	friend class AOEPrototype;
public:
	using CollisionQuery = std::pair<Collider*, std::function<void()>>;
	int GetAttack() const { return attack_; }
	void Update(float elapsed, const std::vector<CollisionQuery>& collision_queries);
	void Draw(DrawSprites& draw_sprite) const;
	void Destroy() { destroyed_ = true; }
	bool IsDestroyed() { return destroyed_; }

	static AOE* CreateMonsterAOE(Room& room, const glm::vec4& bounding_box, Transform2D& transform, int attack);
	static AOE* CreateMapAOE(Room& room, const glm::vec4& bounding_box, int attack);
	
private:
	/**
	 * @param box Bounding box of the AOE for collision detection.
	 * @param animation Animation of the AOE.
	 * @param velocity Velocity of the AOE.
	 * @param duration Duration of the AOE, for number less than 0.0f, it will remain forever.
	 * @param attack The power of the AOE.
	 * @param initial_pos The initial offset from the parent_transform or world origin.
	 * @param penetrate If the AOE can pierce through entities.
	 * @param parent_transform The transform the AOE is moving with.
	 */
	AOE(const glm::vec4& box, Animation* animation, const glm::vec2& velocity, float duration, int attack, const glm::vec2& initial_pos, bool penetrate, Transform2D* parent_transform = nullptr);

	Transform2D transform_;
	Collider collider_;
	glm::vec2 velocity_;
	float duration_;
	AnimationController animation_controller_;
	int attack_;
	bool penetrate_;

	bool destroyed_ { false };
};
