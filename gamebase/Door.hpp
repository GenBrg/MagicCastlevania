#pragma once

#include "Room.hpp"

class Door {
private:
	Animation animation;
	Room* nextRoomP;

	// a field to represent if this door is being open

private:
	Room* getRoom();

};