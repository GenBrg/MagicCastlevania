#include "Door.hpp"

#include <Load.hpp>
#include <engine/Timer.hpp>

#include <iostream>

Animation* Door::opened_animation_ { nullptr };
Animation* Door::opening_animation_ { nullptr };
Animation* Door::closed_animation_ { nullptr };
const Sprite* Door::lock_sprite_;

Door::Door(const glm::vec2& position, Door* opposite_door, Room& room, LockStatus lock_status) :
Entity(kBoundingBox, nullptr),
opposite_door_(opposite_door),
room_(room),
animation_controller_(&transform_)
{
	transform_.position_ = position;
	lock_transform_.position_ = lock_position;
	SetLockStatus(lock_status);
}

void Door::UpdateImpl(float elapsed)
{
	if (lock_status_ != LockStatus::CLOSED) {
		animation_controller_.Update(elapsed);
	}
}

void Door::DrawImpl(DrawSprites& draw)
{
	if (lock_status_ != LockStatus::CLOSED) {
		animation_controller_.Draw(draw);

		if (lock_status_ == LockStatus::LOCKED || lock_status_ == LockStatus::BOSS_LOCKED) {
			draw.draw(*lock_sprite_, lock_transform_);
		}
	}
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
		case LockStatus::OPENED:
		case LockStatus::BOSS_OPENED:
		animation_controller_.PlayAnimation(opened_animation_, false, true);
		break;
		case LockStatus::BOSS_LOCKED:
		case LockStatus::UNLOCK:
		case LockStatus::LOCKED:
		animation_controller_.PlayAnimation(closed_animation_, false, true);
		break;
		case LockStatus::BOSS_OPENING:
		animation_controller_.PlayAnimation(opening_animation_, false, true);
		TimerManager::Instance().AddTimer(Door::opening_animation_->GetLength(), [&](){
			if (lock_status_ == LockStatus::BOSS_OPENING) {
				SetLockStatus(Door::LockStatus::BOSS_OPENED);
			}
		});
		break;
		case LockStatus::OPENING:
		animation_controller_.PlayAnimation(opening_animation_, false, true);
		TimerManager::Instance().AddTimer(Door::opening_animation_->GetLength(), [&](){
			if (lock_status_ == LockStatus::OPENING) {
				SetLockStatus(Door::LockStatus::OPENED);
			}
		});
		break;
		case LockStatus::CLOSED:
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