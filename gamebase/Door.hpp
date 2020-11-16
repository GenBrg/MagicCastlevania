// #pragma once

// #include "Room.hpp"

// class Door {
// private:
// 	Animation animation;
// 	Room* nextRoomP;

// 	// a field to represent if this door is being open

// private:
// 	Room* getRoom();

// };

#pragma once

#include <engine/Animation.hpp>
#include <engine/Trigger.hpp>
#include <engine/Transform2D.hpp>


class Door {
public:
	enum LockStatus : uint8_t {
		UNLOCK = 0,
		NORMAL,
		SPECIAL,
		CLOSED
	};

	Door(const glm::vec2& position, Door* opposite_door, Room& room, LockStatus lock_status);

	void OnOpen(); 

private:
	Transform2D transform_;
	Door* opposite_door_;
	Room& room_;
	AnimationController animation_controller_;
	LockStatus lock_status_;

	static Animation* animation_;
};