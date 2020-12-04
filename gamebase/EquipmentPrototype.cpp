#include "EquipmentPrototype.hpp"

#include <gamebase/Player.hpp>

#include <fstream>
#include <iostream>

void EquipmentPrototype::LoadConfig(const std::string& equipment_list_file)
{
	std::ifstream f(equipment_list_file);
	if (!f.is_open()) {
		throw std::runtime_error("Can not load equipment list file: " + equipment_list_file);
	}

	json j;
	f >> j;

	for (const auto& equipment_json : j) {
		EquipmentPrototype* equipment_prototype = new EquipmentPrototype();
		equipment_prototype->name_ = equipment_json.at("name").get<std::string>();
		std::cout << "Load Equipment: " << equipment_prototype->name_ << std::endl;
		equipment_prototype->description_ = equipment_json.at("description").get<std::string>();
		equipment_prototype->attack_ = equipment_json.at("attack").get<int>();
		equipment_prototype->defense_ = equipment_json.at("defense").get<int>();
		equipment_prototype->slot_ = equipment_json.at("slot").get<int>();

		json icon_sprite_json = equipment_json.at("icon_sprite");
		if (!icon_sprite_json.is_null()) {
			equipment_prototype->icon_sprite_ = &(sprites->lookup(icon_sprite_json.get<std::string>()));
		}
		
		json pickup_sprite_json = equipment_json.at("pickup_sprite");
		if (!pickup_sprite_json.is_null()) {
			equipment_prototype->icon_sprite_ = &(sprites->lookup(pickup_sprite_json.get<std::string>()));
		}
		AddPrototype(equipment_prototype->name_, equipment_prototype);
	}
}

void EquipmentPrototype::Apply(Player* player)
{

}

int EquipmentPrototype::ApplyAttack(int attack)
{
	return attack + attack_;
}

int EquipmentPrototype::ApplyDefense(int defense)
{
	return defense + defense_;
}
