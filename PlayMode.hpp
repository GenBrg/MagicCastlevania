#pragma once
#include "Mode.hpp"

#include <Sound.hpp>
#include <gamebase/Player.hpp>
#include <gamebase/HeadsUpDisplay.hpp>
#include <gamebase/Dialog.hpp>
#include <gamebase/Room.hpp>
#include <gamebase/Shop.hpp>
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

	virtual void on_enter() override;
	virtual void on_leave() override;

	void SwitchRoom(Door* door);
	void ProceedLevel();
	void GenerateRooms();

	/**
	 * Recursively generate rooms in a tree structure way.
	 * @param candidates Available room numbers.
	 * @param remaining_room Remaining rooms to be generated in the tree of rooms which rooted at the room.
	 * @param depth Current depth of the room which can be useful to generate different types of rooms at different depth.
	 * @return The door the newly generated room uses to connect to the parent room.
	 */
	Door* GenerateRoomsHelper(std::vector<int>& candidates, size_t remaining_room, size_t depth);
	void OpenDoor();
	void CollectKey() { ++keys_collected; }
	int GetKeysCollected() const { return keys_collected; }
	int GetTotalKeysToCollect() const { return total_keys_to_collect; }

	//----- game state -----

	std::vector<Room*> rooms;
	Room* cur_room;

	Door* cur_door { nullptr };
	Text press_w_hint;
	TimerGuard open_door_guard;
	size_t level_ { 0 };

	int keys_collected { 0 };
	int total_keys_to_collect { 0 };

	Shop shop;

	std::shared_ptr<Sound::PlayingSample> bgm;
};
