
#include "Room.hpp"

#include <Util.hpp>
#include <engine/Trigger.hpp>
#include <gamebase/RoomPrototype.hpp>
#include <gamebase/Player.hpp>
#include <gamebase/Monster.hpp>

#include <fstream>
#include <iostream>
#include <utility>

#include <iostream>

template <typename T>
void ClearData(std::vector<T>& data) 
{
	for (T& d : data) {
		delete d;
	}
	data.clear();
}

template <typename T>
void GarbageCollect(std::vector<T*>& arr) {
	arr.erase(std::remove_if(arr.begin(), arr.end(), [](T* elem){
		if (elem->IsDestroyed()) {
			delete elem;
			return true;
		}
		return false;
	}),arr.end());
}

/**
 * Constructor of a room
 * @param platform_file the position of platforms in this room
 */
Room::Room(const RoomPrototype& room_prototype):
room_prototype_(room_prototype),
camera_(nullptr)
{
	camera_.position_ = { 0.0f, 0.0f };
}

Room::~Room() {
	ClearData(doors_);
	ClearData(monsters_);
	ClearData(player_AOEs_);
	ClearData(monster_AOEs_);
	ClearData(platforms_);
	ClearData(triggers_);
	ClearData(dialogs_);
}

void Room::Update(float elapsed, Player* player, Door** cur_door)
{
	*cur_door = nullptr;
	player->Update(elapsed);
	player->UpdatePhysics(elapsed, platforms_);

	for (Monster* monster : monsters_) {
		monster->Update(elapsed);
	}

	for (AOE* player_AOE : player_AOEs_) {
		std::vector<AOE::CollisionQuery> collision_queries;
		for (Monster* monster : monsters_) {
			collision_queries.emplace_back(monster->GetCollider(), [=](){
				monster->TakeDamage(player_AOE->GetAttack());
			});
		}	

		player_AOE->Update(elapsed, collision_queries);
	}

	for (AOE* monster_AOE : monster_AOEs_) {
		monster_AOE->Update(elapsed, { std::make_pair(player->GetCollider(), [=](){
			player->TakeDamage(monster_AOE->GetAttack());
		}) });
	}

	for (Trigger* trigger : triggers_) {
		if (trigger->GetCollider()->IsColliding(*(player->GetCollider()))) {
			trigger->OnTrigger();
		}
	}

	for (Door* door : doors_) {
		door->Update(elapsed);
		if (door->GetCollider()->IsColliding(*(player->GetCollider()))) {
			*cur_door = door;
		}
	}

	// Garbage collection
	GarbageCollect(monsters_);
	GarbageCollect(player_AOEs_);
	GarbageCollect(monster_AOEs_);
	GarbageCollect(triggers_);

	// if needs to update dialog or reset it
	if (cur_dialog) {
		if (cur_dialog->ShouldExitDialog()) {
			cur_dialog->UnregisterKeyEvents();
			cur_dialog = nullptr;
		} else {
			cur_dialog->Update(elapsed);
		}
	}
}

void Room::Draw(DrawSprites& draw_sprite)
{
	draw_sprite.draw(*background_sprite_, camera_);

	for (Door* door : doors_) {
		door->Draw(draw_sprite);
	}

	for (Monster* monster : monsters_)
	{
		monster->Draw(draw_sprite);
	}

	for (const AOE* player_AOE : player_AOEs_) {
		player_AOE->Draw(draw_sprite);
	}

	for (const AOE* monster_AOE : monster_AOEs_) {
		monster_AOE->Draw(draw_sprite);
	}
}

void Room::OnEnter(Player* player, Door* door)
{
	room_prototype_.Initialize(this);
	player->SetPosition(door->GetPosition());
}

void Room::OnLeave()
{
	ClearData(monster_AOEs_);
	ClearData(player_AOEs_);
	ClearData(monsters_);
}
