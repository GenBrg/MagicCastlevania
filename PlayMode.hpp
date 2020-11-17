#pragma once
#include "Mode.hpp"

#include <gamebase/Player.hpp>
#include <gamebase/HeadsUpDisplay.hpp>
#include <gamebase/Dialog.hpp>
#include <gamebase/Room.hpp>
#include <engine/Text.hpp>
#include <engine/TimerGuard.hpp>

#include <glm/glm.hpp>

#include <vector>
#include <deque>



struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &window_size) override;

	void SwitchRoom(Door* door);
	void ProceedLevel();
	void GenerateRooms();
	void OpenDoor();

	//----- game state -----

	std::vector<Room*> rooms;
	Room* cur_room;
	Player* player;
	HeadsUpDisplay hud;

	Dialog* dialog_p;
	Door* cur_door { nullptr };
	Text press_w_hint;
	TimerGuard open_door_guard;
	size_t level_ { 0 };
};
