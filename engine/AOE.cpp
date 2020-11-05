#include "AOE.hpp"

AOE::AOE(const glm::vec4& box, Sprite* sprite, const glm::vec2& velocity,
	 float duration, int attack, Transform2D* parent_transform) :
transform_(parent_transform),
collider_(box, &transform_),
velocity_(velocity),
duration_(duration),
sprite_(sprite),
attack_(attack)
{}

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

		if (duration_ >= 0.0f) {
			if (duration_ <= elapsed) {
				duration_ = 0.0f;
				Destroy();
			} else {
				duration_ -= elapsed;
			}
		}
	}
}

void AOE::Draw(DrawSprites& draw) const
{
	if (sprite_) {
		draw.draw(*sprite_, transform_);
	}
}
