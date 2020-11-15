#include "RoomPrototype.hpp"

#include <engine/AOE.hpp>
#include <data_path.hpp>

#include <fstream>
#include <stdexcept>

void RoomPrototype::LoadConfig(const std::string& room_list_file)
{
	std::ifstream f(room_list_file);
	if (!f.is_open()) {
		throw std::runtime_error("Can not load room list: " + room_list_file);
	}

	std::string room_name;

	while (f >> room_name) {
		std::string room_name_filename = data_path(room_name + ".json");
		std::ifstream room_file(room_name_filename);

		if (!room_file.is_open()) {
			throw std::runtime_error("Can not load room file: " + room_name_filename);
		}

		std::cout << "Load room file: " + room_name_filename << std::endl;

		if (prototypes_.count(room_name) > 0) {
			throw std::runtime_error("Duplicate room name: " + room_name);
		}
		RoomPrototype& room_prototype = prototypes_[room_name];

		json j;
		room_file >> j;
		
		for (const auto& platform_json : j.at("platforms")) {
			room_prototype.platforms_.push_back(util::AssetSpaceToGameSpace(platform_json.get<glm::vec4>()));
		}

		for (const auto& monster_json : j.at("monsters")) {
			room_prototype.monsters_.push_back(monster_json.get<MonsterInfo>());
		}

		for (const auto& trap_json : j.at("traps")) {
			room_prototype.traps_.push_back(trap_json.get<Trap>());
		}

		room_prototype.background_sprite_ = &(sprites->lookup(j.at("background_sprite").get<std::string>()));
	}
}

Room* RoomPrototype::Create() const
{
	// Platforms, items, triggers
	Room* room = new Room(*this);
	
	room->background_sprite_ = background_sprite_;

	for (const auto& platform : platforms_) {
		room->platforms_.emplace_back(new Collider(platform, nullptr));
	}

	for (const auto& trap : traps_) {
		AOE::CreateMapAOE(*room, trap.bounding_box_, trap.attack_);
	}

	return room;
}

void RoomPrototype::Initialize(Room* room) const
{
	// Monsters
	for (const auto& monster : monsters_) {
		monster.monster_prototype_->Create(*room, monster.initial_pos_, monster.move_radius_);
	}
}
