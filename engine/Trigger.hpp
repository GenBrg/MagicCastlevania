#pragma once

#include "Entity.hpp"
#include "../DrawSprites.hpp"
#include "../gamebase/Room.hpp"

#include <functional>

/** Trigger class that calls the callback when the user moves into the trigger
 * @author Jiasheng Zhou
 */
class Trigger : public Entity {
public:
	/** Create a new trigger in a room.
	 * 	@param room The room the trigger belongs to.
	 *  @param bounding_box The collision area of the trigger.
	 *  @param transform The parent transform of the trigger.
	 *  @param hit_time_remain How many times the trigger can be triggered. 0 or negative number will result in infinite times.
	 *  @param on_trigger_ The callback when player moves on to the trigger.
	 */
	static Trigger* Create(Room& room, const glm::vec4& bounding_box, Transform2D* transform, int hit_time_remain, float interval_between_hit, const std::function<void()>& on_trigger_);
	void UpdatePhysics(const Collider& collider_to_consider);

private:
	virtual void UpdateImpl(float elapsed) override;
	virtual void DrawImpl(DrawSprites& draw) override;

	int hit_time_remain_;
	std::function<void()> on_trigger_;
	std::function<void()> on_leave_;
	float interval_between_hit_;
	bool is_triggering_ { false };

	Trigger(const glm::vec4& bounding_box, Transform2D* transform, int hit_time_remain, float interval_between_hit, const std::function<void()>& on_trigger);
};
