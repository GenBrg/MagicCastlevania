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

#include <random>
#include <iostream>
#include <sstream>

PlayMode::PlayMode() :
player(Player::Create(&cur_room, data_path("player.json"))),
hud(player)
{
	ProceedLevel();
	std::string test_str = "1abcdefghijklmnopqrstuvwxyz\"~!!!\n2abcdefghijklmnopqrstuvwxyz\"~!!!\n3abcdefghijklmnopqrstuvwxyz\"~!!!\n4abcdefghijklmnopqrstuvwxyz\"~!!!\n5abcdefghijklmnopqrstuvwxyz\"~!!!\n66666666666666\n7abcdefghijklmnopqrstuvwxyz\"~!!!\n888888888888888";
	dialog_p = new Dialog(test_str, "avatar_player", false);
}

PlayMode::~PlayMode() {
	delete dialog_p;

	for (Room* room : rooms) {
		delete room;
	}

	delete player;
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	return InputSystem::Instance()->OnKeyEvent(evt);
}

void PlayMode::update(float elapsed) {
	InputSystem::Instance()->Update(elapsed);
	TimerManager::Instance().Update();

	cur_room->Update(elapsed, player);
	dialog_p->Update(elapsed);
}

void PlayMode::draw(glm::uvec2 const &window_size) {
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

	{ //Overlay some text:
		// if(!dialog_p->ShouldExitDialog()) {
		// 	dialog_p->Draw(window_size);
		// }
	}
	GL_ERRORS();
}

void PlayMode::SwitchRoom(Room* room)
{
	cur_room->OnLeave();
	cur_room = room;
	cur_room->OnEnter(player);
}

void PlayMode::ProceedLevel()
{
	++level_;
	for (Room* room : rooms) {
		delete room;
	}
	rooms.clear();
	GenerateRooms();
	cur_room = rooms[0];
	cur_room->OnEnter(player);
}

void PlayMode::GenerateRooms()
{
	// Room 0 lobby
	// Room 1 BOSS room
	rooms.push_back(RoomPrototype::GetRoomPrototype("room1")->Create());
}
