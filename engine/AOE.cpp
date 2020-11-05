#include "AOE.hpp"

#include "../Util.hpp"

AOE::AOE(const glm::vec4& box, const Sprite* sprite, const glm::vec2& velocity,
 float duration, int attack, const glm::vec2& initial_pos, Transform2D* parent_transform) :
transform_(parent_transform),
collider_(box, &transform_),
velocity_(velocity),
duration_(duration),
sprite_(sprite),
attack_(attack)
{
	transform_.position_ = initial_pos;
}

void AOE::Update(float elapsed, const std::vector<CollisionQuery>& collision_queries)
{
	if (velocity_.x == 0.0f && velocity_.y == 0.0f) {
		// Static collision detection
		for (const CollisionQuery& collision_query : collision_queries) {
			if (collider_.IsColliding(*collision_query.first)) {
				collision_query.second();
			}
		}
	} else {
		// Dynamic collision detection
		glm::vec2 delta_position = velocity_ * elapsed;
		glm::vec2 contact_point, contact_normal;
		float time;

		for (const CollisionQuery& collision_query : collision_queries) {
			if (collider_.DynamicCollisionQuery(*collision_query.first, delta_position, contact_point, contact_normal, time)) {
				collision_query.second();
			}
		}

		transform_.position_ += delta_position;
	}

	if (duration_ >= 0.0f) {
		if (duration_ <= elapsed) {
			duration_ = 0.0f;
			Destroy();
		} else {
			duration_ -= elapsed;
		}
	}
}

void AOE::Draw(DrawSprites& draw) const
{
	if (sprite_) {
		draw.draw(*sprite_, transform_);
	}
}
