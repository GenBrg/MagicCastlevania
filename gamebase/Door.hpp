#pragma once

#include <engine/Animation.hpp>
#include <engine/Trigger.hpp>
#include <engine/Transform2D.hpp>
#include <DrawSprites.hpp>
#include <Util.hpp>

/**
 * Door in the room.
 * @author Jiasheng Zhou
 */
class Door : public Entity {
public:
	enum LockStatus : uint8_t {
		UNLOCK = 0,
		OPENING,
		OPENED,
		NORMAL_LOCKED,
		SPECIAL_LOCKED,
		CLOSED
	};

	static Door* Create(Room& room, const glm::vec2& position);

	void ConnectTo(Door* opposite_door, LockStatus lock_type);
	void SetLockStatus(LockStatus lock_status);
	Door* GetOppositeDoor() const { return opposite_door_; }
	LockStatus GetLockStatus() const { return lock_status_; }
	Room* GetRoom() const { return &room_; }
	glm::vec2 GetPosition() const { return transform_.position_; }

	void OnOpen();

	virtual void UpdateImpl(float elapsed);
	virtual void DrawImpl(DrawSprites& draw);

	static Animation* opened_animation_;
	static Animation* opening_animation_;
	static Animation* closed_animation_;
	static const Sprite* lock_sprite_;

	inline static const glm::vec4 kBoundingBox { 0.0f, 0.0f, 76.0f, 82.0f };
	inline static const glm::vec2 lock_position { 0.0f, 32.0f };

private:
	Door* opposite_door_;
	Room& room_;
	AnimationController animation_controller_;
	LockStatus lock_status_;
	Transform2D lock_transform_ { &transform_ };

	/**
	 * @param position Anchor of the door.
	 * @param opposite_door The door this door is connected to, nullptr if there is no door in the oppsite side.
	 * @param room The room the door resides in.
	 * @param lock_status The lock status of the door.
	 */
	Door(const glm::vec2& position, Door* opposite_door, Room& room, LockStatus lock_status);
};
