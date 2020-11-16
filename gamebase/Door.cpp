#include "Door.hpp"

#include <Load.hpp>

Animation* Door::opened_animation_ { nullptr };
Animation* Door::opening_animation_ { nullptr };
Animation* Door::closed_animation_ { nullptr };

Load<void> load_animations(LoadTagLate, [](){
	Door::opened_animation_ = Animation::GetAnimation("door");
	Door::opening_animation_ = Animation::GetAnimation("door");
	Door::closed_animation_ = Animation::GetAnimation("door");
});

Door::Door(const glm::vec2& position, Door* opposite_door, Room& room, LockStatus lock_status) :
Entity(kBoundingBox, nullptr),
opposite_door_(opposite_door),
room_(room),
animation_controller_(&transform_),
lock_status_(lock_status)
{
	transform_.position_ = position;
	switch (lock_status_) {
		case LockStatus::CLOSED:
		animation_controller_.PlayAnimation(closed_animation_, false, true);
		break;
		case LockStatus::LOCKED:
		animation_controller_.PlayAnimation(closed_animation_, false, true);
		break;
		case LockStatus::UNLOCK:
		animation_controller_.PlayAnimation(opened_animation_, false, true);
		break;
		default:;
	}
}

void Door::UpdateImpl(float elapsed)
{
	animation_controller_.Update(elapsed);
}

void Door::DrawImpl(DrawSprites& draw)
{
	animation_controller_.Draw(draw);
}

void from_json(const json& j, DoorInfo& door_info)
{
	door_info.position_ = util::AssetSpaceToGameSpace(j.at("position").get<glm::vec2>());
	std::string lock_status = j.at("lock_status");
	if (lock_status == "open") {
		door_info.lock_status_ = Door::LockStatus::UNLOCK;
	} else if (lock_status == "close") {
		door_info.lock_status_ = Door::LockStatus::CLOSED;
	} else if (lock_status == "locked") {
		door_info.lock_status_ = Door::LockStatus::LOCKED;
	} else {
		throw std::runtime_error("Unknown lock status: " + lock_status);
	}
	
}

Door* DoorInfo::Create(Room& room) const
{
	Door* door = new Door(position_, nullptr, room, lock_status_);
	room.AddDoor(door);
	return door;
}
