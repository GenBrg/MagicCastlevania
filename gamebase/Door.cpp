#include "Door.hpp"

#include <Load.hpp>
#include <iostream>

Animation* Door::opened_animation_ { nullptr };
Animation* Door::opening_animation_ { nullptr };
Animation* Door::closed_animation_ { nullptr };

Door::Door(const glm::vec2& position, Door* opposite_door, Room& room, LockStatus lock_status) :
Entity(kBoundingBox, nullptr),
opposite_door_(opposite_door),
room_(room),
animation_controller_(&transform_)
{
	transform_.position_ = position;
	SetLockStatus(lock_status);
}

void Door::UpdateImpl(float elapsed)
{
	animation_controller_.Update(elapsed);
}

void Door::DrawImpl(DrawSprites& draw)
{
	animation_controller_.Draw(draw);
}

Door* Door::Create(Room& room, const glm::vec2& position)
{
	Door* door = new Door(position, nullptr, room, LockStatus::CLOSED);
	room.AddDoor(door);
	return door;
}

void Door::SetLockStatus(LockStatus lock_status)
{
	lock_status_ = lock_status;
	switch (lock_status_) {
		case LockStatus::CLOSED:
		animation_controller_.PlayAnimation(closed_animation_, false, true);
		break;
		case LockStatus::NORMAL_LOCKED:
		animation_controller_.PlayAnimation(closed_animation_, false, true);
		break;
		case LockStatus::UNLOCK:
		animation_controller_.PlayAnimation(opened_animation_, false, true);
		break;
		default:
		throw std::runtime_error("Unknown lock status: " + std::to_string(static_cast<uint8_t>(lock_status_)));
	}
}

void Door::ConnectTo(Door* opposite_door, LockStatus lock_type) {
	assert(lock_type != LockStatus::CLOSED);
	opposite_door_ = opposite_door; 
	opposite_door_->SetLockStatus(lock_type);
	opposite_door->opposite_door_ = this;
	SetLockStatus(lock_type);
}