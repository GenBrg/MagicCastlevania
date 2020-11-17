#pragma once
#include "Mode.hpp"
#include "gamebase/Player.hpp"
#include "gamebase/HeadsUpDisplay.hpp"

#include "engine/Text.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include "gamebase/Dialog.hpp"
#include "gamebase/Room.hpp"


struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &window_size) override;

	void SwitchRoom(Room* room);
	void ProceedLevel();
	void GenerateRooms();

	//----- game state -----

	std::vector<Room*> rooms;
	Room* cur_room;
	Player* player;
	HeadsUpDisplay hud;

	size_t level_ { 0 };
};
