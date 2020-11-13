#include "Trigger.hpp"

#include <iostream>

Trigger* Trigger::Create(Room& room, const glm::vec4& bounding_box, Transform2D* transform, int hit_time_remain, float interval_between_hit, const std::function<void()>& on_trigger_)
{
	Trigger* trigger = new Trigger(bounding_box, transform, hit_time_remain, interval_between_hit, on_trigger_);
	room.AddTrigger(trigger);
	return trigger;
}

void Trigger::OnTrigger()
{
	if (IsDestroyed()) {
		return;
	}

	trigger_guard_(interval_between_hit_, [&](){
		on_trigger_();

		if (--hit_time_remain_ == 0) {
			Destroy();
		}
	});
}

void Trigger::UpdateImpl(float elapsed)
{

}

void Trigger::DrawImpl(DrawSprites& draw)
{

}

Trigger::Trigger(const glm::vec4& bounding_box, Transform2D* transform, int hit_time_remain, float interval_between_hit, const std::function<void()>& on_trigger) :
Entity(bounding_box, transform),
hit_time_remain_(hit_time_remain),
on_trigger_(on_trigger),
interval_between_hit_(interval_between_hit)
{

}
