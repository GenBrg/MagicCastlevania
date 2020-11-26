#include "PlayMode.hpp"

#include <DrawLines.hpp>
#include <gl_errors.hpp>
#include <data_path.hpp>
#include <Load.hpp>
#include <Sprite.hpp>
#include <DrawSprites.hpp>
#include <Util.hpp>
#include <engine/Timer.hpp>
#include <engine/Random.hpp>
#include <gamebase/RoomPrototype.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <SDL.h>

#include <random>
#include <iostream>
#include <sstream>

PlayMode::PlayMode() : player(Player::Create(&cur_room, data_path("player.json"))),
					   hud(player),
					   press_w_hint(data_path("ReallyFree-ALwl7.ttf"))
{
	ProceedLevel();

	press_w_hint.SetText("Press W to enter room").SetFontSize(2300).SetPos({0.0f, 30.0f});

	InputSystem::Instance()->Register(SDLK_w, [&](InputSystem::KeyState key_state, float elapsed) {
		if (key_state.pressed)
		{
			key_state.pressed = false;
			OpenDoor();
		}
	});
}

PlayMode::~PlayMode()
{
	for (Room *room : rooms)
	{
		delete room;
	}

	delete player;
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size)
{
	return InputSystem::Instance()->OnKeyEvent(evt);
}

void PlayMode::update(float elapsed)
{
	InputSystem::Instance()->Update(elapsed);
	TimerManager::Instance().Update();

	cur_room->Update(elapsed, player, &cur_door);
}

void PlayMode::draw(glm::uvec2 const &window_size)
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//use alpha blending:
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//don't use the depth test:
	glDisable(GL_DEPTH_TEST);

	{ //use a DrawSprites to do the drawing:
		DrawSprites draw(*sprites, VIEW_MIN, VIEW_MAX, window_size, DrawSprites::AlignPixelPerfect);
		cur_room->Draw(draw);
		player->Draw(draw);
		hud.Draw(draw);
	}

	{
		if (cur_door && cur_door->GetLockStatus() != Door::LockStatus::CLOSED)
		{

			press_w_hint.Draw();
		}

		if (cur_room->cur_dialog)
		{
			// draw it in the end to put in the front layer
			cur_room->cur_dialog->Draw(window_size);
		}
	}
	GL_ERRORS();
}

void PlayMode::SwitchRoom(Door *door)
{
	if (door->GetOppositeDoor() && door->GetLockStatus() == Door::LockStatus::OPENED)
	{
		cur_room->OnLeave();
		Door *opposite_door = door->GetOppositeDoor();
		opposite_door->SetLockStatus(Door::LockStatus::OPENED);
		cur_room = opposite_door->GetRoom();
		cur_room->OnEnter(player, opposite_door);
	}
}

void PlayMode::ProceedLevel()
{
	++level_;
	for (Room *room : rooms)
	{
		delete room;
	}
	rooms.clear();
	GenerateRooms();
	cur_room = rooms[0];
	cur_room->OnEnter(player, cur_room->GetDoor(0));
}

void PlayMode::GenerateRooms()
{
	// Room 0 lobby
	// Room 1 BOSS room
	// rooms.push_back(RoomPrototype::GetRoomPrototype("room1")->Create());
	// rooms.push_back(RoomPrototype::GetRoomPrototype("room2")->Create());
	// rooms.push_back(RoomPrototype::GetRoomPrototype("room3")->Create());
	// rooms[1]->GetDoor(0)->ConnectTo(rooms[0]->GetDoor(0), Door::LockStatus::UNLOCK);
	// rooms[0]->GetDoor(1)->ConnectTo(rooms[2]->GetDoor(0), Door::LockStatus::UNLOCK);
	rooms.push_back(RoomPrototype::GetRoomPrototype("room2")->Create());
	rooms[0]->GetDoor(0)->ConnectTo(GenerateRoomsHelper(false, 1, 7, 1), Door::LockStatus::UNLOCK);
	rooms[0]->GetDoor(1)->ConnectTo(GenerateRoomsHelper(false, 2, 6, 1), Door::LockStatus::UNLOCK);
	rooms[0]->GetDoor(2)->ConnectTo(GenerateRoomsHelper(false, 3, 3, 1), Door::LockStatus::UNLOCK);
}

Door *PlayMode::GenerateRoomsHelper(bool special, int room_id, int remaining_room, int depth)
{
	std::string room_name = (special ? "special_room" : "room") + std::to_string(room_id);
	rooms.push_back(RoomPrototype::GetRoomPrototype(room_name)->Create());
	Room *room = rooms.back();
	int door_num = static_cast<int>(room->GetDoorNum());
	int connecting_door_idx = static_cast<int>(door_num * Random::Instance()->Generate());
	int remaining_door_num = door_num - 1;

	assert(remaining_door_num == 0 || remaining_door_num > 0);

	if (remaining_door_num > 0)
	{
		float average_remaining_room = static_cast<float>(remaining_room) / remaining_door_num;
		size_t room_type_num = RoomPrototype::GetRoomPrototypeNum();

		for (int i = 0; i < door_num && remaining_room > 0; ++i)
		{
			if (i != connecting_door_idx)
			{
				int sub_room_id = -1;
				do
				{
					sub_room_id = 1 + static_cast<int>(room_type_num * Random::Instance()->Generate());
				} while (sub_room_id == room_id);

				if (remaining_door_num == 1)
				{
					room->GetDoor(i)->ConnectTo(GenerateRoomsHelper(false, sub_room_id, remaining_room - 1, depth + 1), Door::LockStatus::UNLOCK);
				}
				else
				{
					int sub_remaining_room = static_cast<int>(average_remaining_room + (2 * Random::Instance()->Generate()) - 1);
					room->GetDoor(i)->ConnectTo(GenerateRoomsHelper(false, sub_room_id, std::clamp(sub_remaining_room - 1, 0, remaining_room), depth + 1), Door::LockStatus::UNLOCK);
				}

				--remaining_door_num;
			}
		}
	}

	return room->GetDoor(connecting_door_idx);
}

void PlayMode::OpenDoor()
{
	open_door_guard(0.5f, [&]() {
		if (cur_door)
		{
			switch (cur_door->GetLockStatus()) {
				case Door::LockStatus::UNLOCK:
					cur_door->SetLockStatus(Door::LockStatus::OPENING);
				break;
				case Door::LockStatus::OPENED:
					SwitchRoom(cur_door);
				break;
				default:;
			}
		}
	});
}
