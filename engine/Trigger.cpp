#include "Trigger.hpp"

#include <iostream>

Trigger* Trigger::Create(Room& room, const glm::vec4& bounding_box, Transform2D* transform, int hit_time_remain)
{
	Trigger* trigger = new Trigger(bounding_box, transform, hit_time_remain);
	room.AddTrigger(trigger);
	return trigger;
}

void Trigger::UpdateImpl(float elapsed)
{

}

void Trigger::DrawImpl(DrawSprites& draw)
{

}

void Trigger::UpdatePhysics(const Collider& collider_to_consider)
{
	if (IsDestroyed()) {
		return;
	}

	if (GetCollider()->IsColliding(collider_to_consider)) {
		if (on_colliding_) {
			on_colliding_();
		}
		if (!is_triggering_) {
			if (on_enter_) {
				on_enter_();
			}
			is_triggering_ = true;
			if (--hit_time_remain_ == 0) {
				Destroy();
			}
		}
	} else {
		if (is_triggering_) {
			if (on_leave_) {
				on_leave_();
			}
			is_triggering_ = false;
		}
	}
}

Trigger::Trigger(const glm::vec4& bounding_box, Transform2D* transform, int hit_time_remain) :
Entity(bounding_box, transform),
hit_time_remain_(hit_time_remain)
{}
