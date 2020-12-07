#include "PotionPrototype.hpp"

#include <gamebase/Player.hpp>
#include <Util.hpp>

#include <fstream>
#include <iostream>

void PotionPrototype::Apply(Player* player)
{
	player->AddHp(hp_);
	player->AddMp(mp_);
	player->AddExp(exp_);
	for (const auto& buff : buffs_) {
		player->AddBuff(buff);
	}
}

void PotionPrototype::LoadConfig(const std::string& potion_list_file)
{
	std::ifstream f(potion_list_file);
	if (!f.is_open()) {
		throw std::runtime_error("Can not load potion list file: " + potion_list_file);
	}

	json j;
	f >> j;

	for (const auto& potion_json : j) {
		PotionPrototype* potion_prototype = new PotionPrototype();
		potion_prototype->name_ = potion_json.at("name").get<std::string>();
		std::cout << "Load Potion: " << potion_prototype->name_ << std::endl;
		potion_prototype->description_ = potion_json.at("description").get<std::string>();
		potion_prototype->hp_ = potion_json.at("hp").get<int>();
		potion_prototype->mp_ = potion_json.at("mp").get<int>();
		potion_prototype->exp_ = potion_json.at("exp").get<int>();
		potion_prototype->price_ = potion_json.at("price").get<int>();

		json icon_sprite_json = potion_json.at("icon_sprite");
		if (!icon_sprite_json.is_null()) {
			potion_prototype->icon_sprite_ = &(sprites->lookup(icon_sprite_json.get<std::string>()));
		}
		
		json pickup_sprite_json = potion_json.at("pickup_sprite");
		if (!pickup_sprite_json.is_null()) {
			potion_prototype->pickup_sprite_ = &(sprites->lookup(pickup_sprite_json.get<std::string>()));
		}

		for (const auto& buff_json : potion_json.at("buffs")) {
			potion_prototype->buffs_.push_back(buff_json.get<Buff>());
		}

		AddPrototype(potion_prototype->name_, potion_prototype);
	}
}
