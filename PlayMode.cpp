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

#define HALF_SWITCH_ROOM_TRANSITION SWITCH_ROOM_TRANSITION / 2.0f

PlayMode::PlayMode() : 
press_w_hint(data_path("ReallyFree-ALwl7.ttf")),
level_transition_scene(sprites->lookup("level_clear"))
{
	
	player = Player::Create(&cur_room, data_path("player.json"));
	hud = new HeadsUpDisplay();
	ProceedLevel();

	press_w_hint.SetText("Press W to enter room").SetFontSize(2300).SetPos({0.0f, 30.0f});

	InputSystem::Instance()->Register(SDLK_w, [&](InputSystem::KeyState key_state, float elapsed) {
		if (key_state.pressed && !cur_room->IsDialoging())
		{
			key_state.pressed = false;
			OpenDoor();
		}
	});

	InputSystem::Instance()->Register(SDLK_ESCAPE, [&](InputSystem::KeyState &key_state, float elapsed) {
		if (key_state.pressed)
		{
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
	InputSystem::Instance()->Register(SDLK_TAB, [&](InputSystem::KeyState& key_state, float elapsed) {
		if (key_state.pressed)
		{
			key_state.pressed = false;
			std::vector< MenuMode::Item > items;
			items.emplace_back("Static", &sprites->lookup("controls_static"), nullptr);
			std::shared_ptr< MenuMode > controls_menu;
			controls_menu = std::make_shared< MenuMode >(items, 0);
			controls_menu->selected = 1;
			controls_menu->atlas = sprites;
			controls_menu->view_min = glm::vec2(0.0f, 0.0f);
			controls_menu->view_max = glm::vec2(960.0f, 541.0f);
			controls_menu->background = main_play;
			Mode::set_current(controls_menu);
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
	level_clear_scene_duration -= elapsed;

    if (in_transition_) {
        elapsed_since_transition_ += elapsed;
        if (elapsed_since_transition_ > trans_time_) {
            in_transition_ = false;
        }
    } else {
        InputSystem::Instance()->Update(elapsed);

        cur_room->Update(elapsed, player, &cur_door);
        hud->Update(elapsed);
    }
	TimerManager::Instance().Update();
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

	if (level_clear_scene_duration > 0.0f) {
		DrawSprites draw(*sprites, VIEW_MIN, VIEW_MAX, window_size, DrawSprites::AlignPixelPerfect);
		Transform2D transform = Transform2D(nullptr);
		draw.draw(level_transition_scene, transform);
	} else {
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

		{
			if (in_transition_) {
				DrawSprites draw(*sprites, VIEW_MIN, VIEW_MAX, window_size, DrawSprites::AlignPixelPerfect);
				Transform2D transform = Transform2D(nullptr);
				transform.position_ = glm::vec2(0.0f, 0.0f);
				transform.scale_ = glm::vec2(10.0f, 10.0f);
				// calculate based on current time
				float alphaPercent = 1.0f - fabs(elapsed_since_transition_ - trans_time_ / 2.0f) / (trans_time_ / 2.0f);
				auto alpha = (uint8_t)(0xff * alphaPercent);
				draw.draw(sprites->lookup("shop_window"), transform, glm::u8vec4(0xff, 0xff, 0xff, alpha));
			}
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
			StartBGM("boss_" + std::to_string(level_));
		}
	}
}

void PlayMode::ProceedLevel()
{
	++level_;

	if (level_ > kMaxLevel) {
		PlayEndScene();
		return;
	}

	ResetCurrentLevel();
}

void PlayMode::GenerateRooms()
{
	total_keys_to_collect = 0;
	std::string level_string = std::to_string(level_);

	rooms.push_back(RoomPrototype::GetRoomPrototype("main_lobby" + level_string)->Create(level_));
	rooms.push_back(RoomPrototype::GetRoomPrototype("boss_room" + level_string)->Create(level_));

	rooms[0]->GetDoor(2)->ConnectTo(rooms[1]->GetDoor(0), Door::LockStatus::SPECIAL_LOCKED);

	// Randomly generate rooms behind first two doors
	std::vector<int> candidate_rooms;
	for (size_t i = 3; i <= RoomPrototype::kMaxRoomNumber; ++i) {
		candidate_rooms.push_back(static_cast<int>(i));
	}

	if (level_ == 1) {
		#define _DEBUG
#ifdef _DEBUG
	// For Test
	rooms.push_back(RoomPrototype::GetRoomPrototype("room2-3")->Create(level_));
	rooms[0]->GetDoor(0)->ConnectTo(rooms[2]->GetDoor(0), Door::LockStatus::UNLOCK);
#else
	rooms.push_back(RoomPrototype::GetRoomPrototype("tutorial_room")->Create(level_));
	rooms[0]->GetDoor(0)->ConnectTo(rooms[2]->GetDoor(0), Door::LockStatus::UNLOCK);
	
	size_t door1_room_num = candidate_rooms.size() / 2;
	size_t door2_room_num = candidate_rooms.size() - door1_room_num;
	rooms[2]->GetDoor(1)->ConnectTo(GenerateRoomsHelper(level_string, candidate_rooms, door1_room_num, 1), Door::LockStatus::UNLOCK);
	rooms[2]->GetDoor(2)->ConnectTo(GenerateRoomsHelper(level_string, candidate_rooms, door2_room_num, 1), Door::LockStatus::UNLOCK);
#endif

		 
		
	} else if (level_ <= kMaxLevel) {
		size_t door1_room_num = candidate_rooms.size() / 2;
		size_t door2_room_num = candidate_rooms.size() - door1_room_num;
		rooms[0]->GetDoor(0)->ConnectTo(GenerateRoomsHelper(level_string, candidate_rooms, door1_room_num, 1), Door::LockStatus::UNLOCK);
		rooms[0]->GetDoor(1)->ConnectTo(GenerateRoomsHelper(level_string, candidate_rooms, door2_room_num, 1), Door::LockStatus::UNLOCK);
	} else {
		throw std::runtime_error("Level exceed max level!");
	}
}

Door *PlayMode::GenerateRoomsHelper(const std::string& level_string, std::vector<int>& candidates, size_t remaining_room, size_t depth)
{
	assert(candidates.size() >= remaining_room);

	auto random_choose_room = [&](){
		size_t candidate_idx = static_cast<size_t>(candidates.size() * Random::Instance()->Generate());
		std::string room_name = "room" + level_string + "-" + std::to_string(candidates[candidate_idx]);
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
					room->GetDoor(i)->ConnectTo(GenerateRoomsHelper(level_string, candidates, remaining_room, depth + 1), Door::LockStatus::UNLOCK);
					remaining_room = 0;
				}
				else
				{
					size_t sub_remaining_room = static_cast<size_t>(average_remaining_room + (2 * Random::Instance()->Generate()) - 1);
					sub_remaining_room = static_cast<size_t>(std::clamp(static_cast<unsigned long long>(sub_remaining_room), 1ull, static_cast<unsigned long long>(remaining_room)));
					room->GetDoor(i)->ConnectTo(GenerateRoomsHelper(level_string, candidates, sub_remaining_room, depth + 1), Door::LockStatus::UNLOCK);
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
					Transition(SWITCH_ROOM_TRANSITION);
					Sound::play(*sound_samples["footstep"]);
					TimerManager::Instance().AddTimer(HALF_SWITCH_ROOM_TRANSITION, [&](){
						SwitchRoom(cur_door);
					});
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

void PlayMode::ResetCurrentLevel()
{
	TimerManager::Instance().ClearAllTimers();
	StartBGM("all_1");
	for (Room *room : rooms)
	{
		delete room;
	}
	rooms.clear();
	keys_collected = 0;
	GenerateRooms();

	cur_room = rooms[0];
	cur_room->OnEnter(player, cur_room->GetDoor(0));
}

void PlayMode::Transition(float trans_time)
{
	in_transition_ = true;
    elapsed_since_transition_ = 0.0f;
    trans_time_ = trans_time;
}

void PlayMode::StopBGM()
{
	if (bgm) {
		bgm->stop();
		bgm = nullptr;
	}
}

void PlayMode::StartBGM(const std::string& bgm_name)
{
	StopBGM();
	bgm = Sound::loop(*sound_samples[bgm_name], 0.0f);
	bgm->set_volume(1.0f, 2.0f);
}

void PlayMode::PlayEndScene()
{
	StartBGM("ending_1");
	std::vector< MenuMode::Item > items;
	items.emplace_back("Static", &sprites->lookup("end_scene"), nullptr);
	std::shared_ptr< MenuMode > ending_menu;
	ending_menu = std::make_shared< MenuMode >(items, 0);
	ending_menu->selected = 1;
	ending_menu->atlas = sprites;
	ending_menu->view_min = glm::vec2(0.0f, 0.0f);
	ending_menu->view_max = glm::vec2(960.0f, 541.0f);
	ending_menu->background = main_play;
	Mode::set_current(ending_menu);
}

void PlayMode::DisplayLevelClearScene(float duration)
{ 
	level_clear_scene_duration = duration;
}