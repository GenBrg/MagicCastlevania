#include "AOE.hpp"
#include "../gamebase/Room.hpp"
#include "../DrawLines.hpp"
#include "../Util.hpp"

AOE::AOE(const glm::vec4& box, Animation* animation, const glm::vec2& velocity, float duration, int attack, const glm::vec2& initial_pos, bool penetrate,
 bool face_right, bool can_damage_same_object, Transform2D* parent_transform) :
transform_(parent_transform),
collider_(box, &transform_),
velocity_(velocity),
duration_(duration),
animation_controller_(&transform_),
attack_(attack),
penetrate_(penetrate),
can_damage_same_object_(can_damage_same_object)
{
	if (!face_right) {
		velocity_.x = -velocity_.x;
		collider_.SetBoundingBox(glm::vec4(-box[2], box[1], -box[0], box[3]));
		if (!parent_transform) {
			transform_.scale_.x = -1.0f;
		}
	}
	transform_.position_ = initial_pos;
	animation_controller_.PlayAnimation(animation, false);
}

void AOE::Update(float elapsed, const std::vector<CollisionQuery>& collision_queries)
{
	if (IsDestroyed()) {
		return;
	}

	animation_controller_.Update(elapsed);

	transform_.position_ += velocity_ * elapsed;
	// Static collision detection
	for (const CollisionQuery& collision_query : collision_queries) {
		Collider* collider_to_consider = collision_query.first;
		if (!can_damage_same_object_ && colliders_interacted_.count(collider_to_consider) > 0) {
			continue;
		}

		if (collider_.IsColliding(*collider_to_consider)) {
			collision_query.second();
			colliders_interacted_.insert(collider_to_consider);

			if (!penetrate_) {
				Destroy();
				return;
			}
		}
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
	// Debug draw bounding box
	collider_.DrawDebugBox(draw);
}

AOE* AOE::CreateMonsterAOE(Room& room, const glm::vec4& bounding_box, Transform2D& transform, int attack)
{
	AOE* aoe = new AOE(bounding_box, nullptr, glm::vec2(0.0f, 0.0f), -1.0f, attack, glm::vec2(0.0f, 0.0f), true, true, true, &transform);
	room.AddMonsterAOE(aoe);
	return aoe;
}

AOE* AOE::CreateMapAOE(Room& room, const glm::vec4& bounding_box, int attack)
{
	AOE* aoe = new AOE(bounding_box, nullptr, glm::vec2(0.0f, 0.0f), -1.0f, attack, glm::vec2(0.0f, 0.0f), true, true, true, nullptr);
	room.AddMonsterAOE(aoe);
	return aoe;
}
