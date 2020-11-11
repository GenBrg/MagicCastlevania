
#include "Room.hpp"
#include "../Util.hpp"
#include "../engine/Trigger.hpp"
#include "Player.hpp"

#include <fstream>
#include <iostream>
#include <utility>

#include <iostream>

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

	// tmp hard code to generate monsters
	monsters_.push_back(new Monster(glm::vec2(505, 198), 60, "ghost_idle_1", this));
	AddMonsterAOE(new AOE(glm::vec4(434.0f, 0.0f, 951.0f, 40.0f), nullptr, glm::vec2(0.0f, 0.0f), -1.0f, 10000, glm::vec2(0.0f, 0.0f), nullptr));

	Trigger::Create(*this, glm::vec4(0.0f, 10.0f, 300.0f, 1000.0f), nullptr, 10, 0.1f, [&](){
		std::cout << "Player in area!!!" << std::endl;
	});
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

void Room::Update(float elapsed, Player* player)
{
	player->Update(elapsed, platforms_);

	for (Monster* monster : monsters_) {
		monster->Update(elapsed, platforms_);
	}

	for (AOE* player_AOE : player_AOEs_) {
		std::vector<AOE::CollisionQuery> collision_queries;
		for (Monster* monster : monsters_) {
			collision_queries.emplace_back(monster->GetCollider(), [&](){
				monster->TakeDamage(player_AOE->GetAttack());
			});
		}	

		player_AOE->Update(elapsed, collision_queries);
	}

	for (AOE* monster_AOE : monster_AOEs_) {
		monster_AOE->Update(elapsed, { std::make_pair(player->GetCollider(), [&](){
			player->TakeDamage(monster_AOE->GetAttack());
		}) });
	}

	for (Trigger* trigger : triggers_) {
		if (trigger->GetCollider()->IsColliding(*(player->GetCollider()))) {
			trigger->OnTrigger();
		}
	}

	// Garbage collection
	GarbageCollect(monsters_);
	GarbageCollect(player_AOEs_);
	GarbageCollect(monster_AOEs_);
	GarbageCollect(triggers_);
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
