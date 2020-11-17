#include "PlayMode.hpp"

#include <DrawLines.hpp>
#include <gl_errors.hpp>
#include <data_path.hpp>
#include <Load.hpp>
#include <Sprite.hpp>
#include <DrawSprites.hpp>
#include <Util.hpp>
#include <engine/Timer.hpp>
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
	dialog_p = new Dialog();
	dialog_p->Append("BACKGROUND STROY111111\nBACKGROUND STROY22222\nBACKGROUND STROY333333\n1abcdefghijklmnopqrstuvwxyz", "avatar_null");
	dialog_p->Append("avatar_assassin IS SPAEKINGSTROY111111\navatar_assassin IS STROY22222\navatar_assassin IS STROY33333", "avatar_assassin");
	dialog_p->Append("PLAYER IS SPEAKINGSTROY111111\nPLAYER IS SPEAKING222222\nPLAYER IS SPEAKING33333\nPLAYER IS SPEAKING444444", "avatar_player");
	dialog_p->RegisterKeyEvents();

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
	delete dialog_p;

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
	dialog_p->Update(elapsed);
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
		if (cur_door)
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
	if (door->GetOppositeDoor() && door->GetLockStatus() == Door::LockStatus::UNLOCK)
	{
		cur_room->OnLeave();
		Door *opposite_door = door->GetOppositeDoor();
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
	cur_room = rooms[1];
	cur_room->OnEnter(player, cur_room->GetDoor(0));
}

void PlayMode::GenerateRooms()
{
	// Room 0 lobby
	// Room 1 BOSS room
	rooms.push_back(RoomPrototype::GetRoomPrototype("room1")->Create());
	rooms.push_back(RoomPrototype::GetRoomPrototype("room2")->Create());
	rooms[1]->GetDoor(0)->SetOppositeDoor(rooms[0]->GetDoor(0));
}

void PlayMode::OpenDoor()
{
	if (cur_door)
	{
		open_door_guard(5.0f, [&]() {
			SwitchRoom(cur_door);
		});
	}
}
