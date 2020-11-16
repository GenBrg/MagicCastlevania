#include "AOE.hpp"
#include "../gamebase/Room.hpp"

#include "../Util.hpp"

AOE::AOE(const glm::vec4& box, Animation* animation, const glm::vec2& velocity, float duration, int attack, const glm::vec2& initial_pos, bool penetrate,
 Transform2D* parent_transform) :
transform_(parent_transform),
collider_(box, &transform_),
velocity_(velocity),
duration_(duration),
animation_controller_(&transform_),
attack_(attack),
penetrate_(penetrate)
{
	transform_.position_ = initial_pos;
	animation_controller_.PlayAnimation(animation, false);
}

void AOE::Update(float elapsed, const std::vector<CollisionQuery>& collision_queries)
{
	if (IsDestroyed()) {
		return;
	}

	if (velocity_.x == 0.0f && velocity_.y == 0.0f) {
		// Static collision detection
		for (const CollisionQuery& collision_query : collision_queries) {
			if (collider_.IsColliding(*collision_query.first)) {
				collision_query.second();

				if (!penetrate_) {
					Destroy();
					return;
				}
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
				
				if (!penetrate_) {
					Destroy();
					return;
				}
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
	animation_controller_.Draw(draw);
}

AOE* AOE::CreateMonsterAOE(Room& room, const glm::vec4& bounding_box, Transform2D& transform, int attack)
{
	AOE* aoe = new AOE(bounding_box, nullptr, glm::vec2(0.0f, 0.0f), -1.0f, attack, glm::vec2(0.0f, 0.0f), true, &transform);
	room.AddMonsterAOE(aoe);
	return aoe;
}

AOE* AOE::CreateMapAOE(Room& room, const glm::vec4& bounding_box, int attack)
{
	AOE* aoe = new AOE(bounding_box, nullptr, glm::vec2(0.0f, 0.0f), -1.0f, attack, glm::vec2(0.0f, 0.0f), true, nullptr);
	room.AddMonsterAOE(aoe);
	return aoe;
}
