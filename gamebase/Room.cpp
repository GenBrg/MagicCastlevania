
#include "Room.hpp"
#include "../Util.hpp"

#include <fstream>
#include <iostream>
#include <utility>

/**
 * Constructor of a room
 * @param platform_file the position of platforms in this room
 */
Room::Room(const std::string& platform_file) {
	std::ifstream in_file(platform_file);
	float top_left_x, top_left_y, relative_x, relative_y;
	std::string platform_name;

	while (in_file >> top_left_x >> top_left_y >> relative_x >> relative_y >> platform_name) {
		glm::vec4 bounding_box(top_left_x, VIEW_MAX.y - top_left_y - relative_y,
						 top_left_x + relative_x,VIEW_MAX.y - top_left_y);

		platforms_.push_back(new Collider(bounding_box, nullptr));
	}
//	for(auto* p: platforms_) {
//		util::PrintVec4(p->GetBoundingBox());
//	}
}

Room::~Room() {
	for(auto p: platforms_) {
		delete p;
	}

	for (AOE* player_AOE : player_AOEs_) {
		delete player_AOE;
	}

	for (AOE* monster_AOE : monster_AOEs_) {
		delete monster_AOE;
	}
}

void Room::Update(float elapsed, Player* player)
{
	player->Update(elapsed, platforms_);

	for (Monster* monster : monsters_) {
		monster->Update(elapsed, platforms_);
	}

	for (AOE* player_AOE : player_AOEs_) {
		for (Monster* monster : monsters_) {
			player_AOE->Update(elapsed, std::make_pair(monster->GetCollider(), [&](){
				monster->TakeDamage(player_AOE->GetAttack());
			}));
		}
	}

	for (AOE* monster_AOE : monster_AOEs_) {
		monster_AOE->Update(elapsed, std::make_pair(player->GetCollider(), [&](){
			player->TakeDamage(monster_AOE->GetAttack());
		}));
	}

	// Recycle memory
	std::remove_if(monsters_.begin(), monsters_.end(), [](AOE* monster){
		if (monster->IsDestroyed()) {
			delete monster;
			return true;
		}
		return false;
	});

	std::remove_if(player_AOEs_.begin(), player_AOEs_.end(), [](AOE* player_AOE){
		if (player_AOE->IsDestroyed()) {
			delete player_AOE;
			return true;
		}
		return false;
	});

	std::remove_if(monster_AOEs_.begin(), monster_AOEs_.end(), [](AOE* monster_AOE){
		if (monster_AOE->IsDestroyed()) {
			delete monster_AOE;
			return true;
		}
		return false;
	});
}

void Room::Draw(DrawSprites& draw_sprite)
{
	for (const Monster* monster : monsters_)
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
