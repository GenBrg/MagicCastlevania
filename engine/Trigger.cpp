#include "Trigger.hpp"

#include <iostream>

Trigger* Trigger::Create(Room& room, const glm::vec4& bounding_box, Transform2D* transform, int hit_time_remain, float interval_between_hit, const std::function<void()>& on_trigger_)
{
	Trigger* trigger = new Trigger(bounding_box, transform, hit_time_remain, interval_between_hit, on_trigger_);
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
		if (!is_triggering_) {
			on_trigger_();
			is_triggering_ = true;
			if (--hit_time_remain_ <= 0) {
				Destroy();
			}
		}
	} else {
		if (is_triggering_) {
			on_leave_();
			is_triggering_ = false;
		}
	}
}

Trigger::Trigger(const glm::vec4& bounding_box, Transform2D* transform, int hit_time_remain, float interval_between_hit, const std::function<void()>& on_trigger) :
Entity(bounding_box, transform),
hit_time_remain_(hit_time_remain),
on_trigger_(on_trigger),
interval_between_hit_(interval_between_hit)
{

}
