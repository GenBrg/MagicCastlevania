#include "PlayMode.hpp"
#include "MenuMode.hpp"
#include "main_play.hpp"
#include "gamebase/Player.hpp"
#include "gamebase/ItemPrototype.hpp"
#include "gamebase/EquipmentPrototype.hpp"
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

PlayMode::PlayMode() : press_w_hint(data_path("ReallyFree-ALwl7.ttf"))
{
	player = Player::Create(&cur_room, data_path("player.json"));
	hud = new HeadsUpDisplay();
	ProceedLevel();

	press_w_hint.SetText("Press W to enter room").SetFontSize(2300).SetPos({0.0f, 30.0f});

	InputSystem::Instance()->Register(SDLK_w, [&](InputSystem::KeyState key_state, float elapsed) {
		if (key_state.pressed)
		{
			key_state.pressed = false;
			OpenDoor();
		}
	});

	InputSystem::Instance()->Register(SDLK_ESCAPE, [&](InputSystem::KeyState &key_state, float elapsed) {
		if (key_state.pressed)
		{
			std::cout << "Pressed" << std::endl;
			key_state.pressed = false;
			std::vector< MenuMode::Item > items;
			items.emplace_back("Static", &sprites->lookup("pause_window"), nullptr);
			for (size_t i = 0; i < 4; i++) {
				items.emplace_back("Equipment_" + std::to_string(i), nullptr, &sprites->lookup("select_target"));
			}
			for (size_t i = 0; i < 12; i++) {
				items.emplace_back("Item_" + std::to_string(i), nullptr, &sprites->lookup("select_target"));
			}
			std::shared_ptr< MenuMode > pause_menu;
			pause_menu = std::make_shared< MenuMode >(items, 4);
			pause_menu->selected = 1;
			pause_menu->atlas = sprites;
			pause_menu->view_min = glm::vec2(0.0f, 0.0f);
			pause_menu->view_max = glm::vec2(960.0f, 541.0f);
			pause_menu->grid_layout_items(glm::vec2(574.0f, 430.0f), 77.0f, 10.0f, 1, 5);
			pause_menu->grid_layout_items(glm::vec2(574.0f, 282.0f), 77.0f, 80.0f, 5, 17);
			Mode::set_current(pause_menu);
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
    delete hud;
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
	hud->Update(elapsed);
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
		hud->Draw(draw);
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
		if (cur_room == rooms[1]) {
			opposite_door->SetLockStatus(Door::LockStatus::NORMAL_LOCKED);
			if (bgm) {
				bgm->stop();
			}
			bgm = Sound::loop(*sound_samples["boss_" + std::to_string(level_)]);
		}
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
	keys_collected = 0;
	GenerateRooms();
	cur_room = rooms[0];
	cur_room->OnEnter(player, cur_room->GetDoor(0));

	if (bgm) {
		bgm->stop();
	}
	bgm = Sound::loop(*sound_samples["all_1"]);
}

void PlayMode::GenerateRooms()
{
	total_keys_to_collect = 0;

	// Room 1 lobby
	// Room 2 BOSS room
	rooms.push_back(RoomPrototype::GetRoomPrototype("room1")->Create(level_));
	rooms.push_back(RoomPrototype::GetRoomPrototype("room2")->Create(level_));

	rooms[0]->GetDoor(2)->ConnectTo(rooms[1]->GetDoor(0), Door::LockStatus::SPECIAL_LOCKED);

	// For Test
	rooms.push_back(RoomPrototype::GetRoomPrototype("room4")->Create(level_));
	rooms[0]->GetDoor(0)->ConnectTo(rooms[2]->GetDoor(0), Door::LockStatus::UNLOCK);

	// Randomly generate rooms behind first two doors
	// std::vector<int> candidate_rooms;
	// for (size_t i = 3; i <= RoomPrototype::GetRoomPrototypeNum(); ++i) {
	// 	candidate_rooms.push_back(static_cast<int>(i));
	// }

	// size_t door1_room_num = candidate_rooms.size() / 2;
	// size_t door2_room_num = candidate_rooms.size() - door1_room_num;
	// rooms[0]->GetDoor(0)->ConnectTo(GenerateRoomsHelper(candidate_rooms, door1_room_num, 1), Door::LockStatus::UNLOCK);
	// rooms[0]->GetDoor(1)->ConnectTo(GenerateRoomsHelper(candidate_rooms, door2_room_num, 1), Door::LockStatus::UNLOCK);
}

Door *PlayMode::GenerateRoomsHelper(std::vector<int>& candidates, size_t remaining_room, size_t depth)
{
	assert(candidates.size() >= remaining_room);

	auto random_choose_room = [&](){
		size_t candidate_idx = static_cast<size_t>(candidates.size() * Random::Instance()->Generate());
		std::string room_name = "room" + std::to_string(candidates[candidate_idx]);
		candidates.erase(candidates.begin() + candidate_idx);
		rooms.push_back(RoomPrototype::GetRoomPrototype(room_name)->Create(level_));
		return rooms.back();
	};

	// Generate current room
	Room *room = random_choose_room();

	int door_num = static_cast<int>(room->GetDoorNum());
	int connecting_door_idx = static_cast<int>(door_num * Random::Instance()->Generate());
	int remaining_door_num = door_num - 1;
	--remaining_room;

	assert(remaining_door_num >= 0);

	if (remaining_room == 0) {
		// Generate key on the leaf node 
		room->GenerateKey();
		++total_keys_to_collect;
	}

	if (remaining_door_num > 0 && remaining_room > 0)
	{
		float average_remaining_room = static_cast<float>(remaining_room) / remaining_door_num;

		for (int i = 0; i < door_num && remaining_room > 0; ++i)
		{
			if (i != connecting_door_idx)
			{
				if (remaining_door_num == 1)
				{
					room->GetDoor(i)->ConnectTo(GenerateRoomsHelper(candidates, remaining_room, depth + 1), Door::LockStatus::UNLOCK);
					remaining_room = 0;
				}
				else
				{
					size_t sub_remaining_room = static_cast<size_t>(average_remaining_room + (2 * Random::Instance()->Generate()) - 1);
					sub_remaining_room = static_cast<size_t>(std::clamp(static_cast<unsigned long long>(sub_remaining_room), 1ull, static_cast<unsigned long long>(remaining_room)));
					room->GetDoor(i)->ConnectTo(GenerateRoomsHelper(candidates, sub_remaining_room, depth + 1), Door::LockStatus::UNLOCK);
					remaining_room -= sub_remaining_room;
				}

				--remaining_door_num;
			}
		}
	}

	assert(remaining_room == 0);
	
	return room->GetDoor(connecting_door_idx);
}

void PlayMode::OpenDoor()
{
	open_door_guard(0.6f, [&]() {
		if (cur_door)
		{
			switch (cur_door->GetLockStatus()) {
				case Door::LockStatus::UNLOCK:
					Sound::play(*sound_samples["open_door"]);
					cur_door->SetLockStatus(Door::LockStatus::OPENING);
				break;
				case Door::LockStatus::OPENED:
					SwitchRoom(cur_door);
				break;
				case Door::LockStatus::SPECIAL_LOCKED:
					if (keys_collected >= total_keys_to_collect) {
						Sound::play(*sound_samples["open_door"]);
						cur_door->SetLockStatus(Door::LockStatus::OPENING);
					}
				break;
				default:;
			}
		}
	});
}

void PlayMode::on_enter() {
	if (bgm) {
		bgm->set_volume(1.0f);
	}
}
void PlayMode::on_leave() {
	if (bgm) {
		bgm->set_volume(0.3f);
	}
}
