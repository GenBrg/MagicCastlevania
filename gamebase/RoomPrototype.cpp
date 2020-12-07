#include "RoomPrototype.hpp"

#include <engine/InputSystem.hpp>
#include <engine/AOE.hpp>
#include <engine/Random.hpp>
#include <gamebase/ItemPrototype.hpp>
#include <data_path.hpp>

#include <fstream>
#include <stdexcept>

std::unordered_map<std::string, RoomPrototype> RoomPrototype::prototypes_;

void RoomPrototype::LoadConfig(const std::string& room_list_file)
{
	std::ifstream f(room_list_file);
	if (!f.is_open()) {
		throw std::runtime_error("Can not load room list: " + room_list_file);
	}

	std::string room_name;

	while (f >> room_name) {
		std::string room_name_filename = data_path("rooms/" + room_name + ".json");
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

		for (const auto& platform_json : j.at("one_sided_platforms")) {
			room_prototype.one_sided_platforms_.push_back(util::AssetSpaceToGameSpace(platform_json.get<glm::vec4>()));
		}

		for (const auto &monster_json : j.at("monsters")) {
			room_prototype.monsters_.push_back(monster_json.get<MonsterInfo>());
		}

		for (const auto &trap_json : j.at("traps")) {
			room_prototype.traps_.push_back(trap_json.get<Trap>());
		}

		for (const auto &dialog_info_json: j.at("dialogs")) {
			room_prototype.dialog_infos_.push_back(dialog_info_json.get<DialogInfo>());
		}

		for (const auto& door_json : j.at("doors")) {
			room_prototype.doors_.push_back(util::AssetSpaceToGameSpace(door_json.get<glm::vec2>()));
		}

		room_prototype.background_sprite_ = &(sprites->lookup(j.at("background_sprite").get<std::string>()));

		for (const auto& item_json : j.at("items")) {
			room_prototype.item_infos_.push_back({ItemPrototype::GetPrototype(item_json.at("name").get<std::string>()),
			 util::AssetSpaceToGameSpace(item_json.at("position").get<glm::vec2>()), item_json.at("probability").get<float>()});
		}

		room_prototype.key_position_ = util::AssetSpaceToGameSpace(j.at("key_position").get<glm::vec2>());

		if (j.contains("shop_position")) {
		    room_prototype.shopInfo_.contain_shop_ = true;
		    room_prototype.shopInfo_.pos_ = util::AssetSpaceToGameSpace(j.at("shop_position").get<glm::vec2>());
		} else {
            room_prototype.shopInfo_.contain_shop_ = false;
		}
	}
}

Room* RoomPrototype::Create(size_t level) const
{
	// Platforms, items, triggers
	Room* room = new Room(*this);

	room->background_sprite_ = background_sprite_;

	for (const auto &platform : platforms_) {
		room->platforms_.emplace_back(new Collider(platform, nullptr));
	}

	for (const auto &platform : one_sided_platforms_) {
		Collider* collider = new Collider(platform, nullptr);
		collider->SetOneSided(true);
		room->platforms_.emplace_back(collider);
	}

	for (const auto &trap : traps_) {
		AOE::CreateMapAOE(*room, trap.bounding_box_, trap.attack_);
	}

	// create dialog object & trigger
	for (auto &dialog_info: dialog_infos_) {
		auto *dialog = new Dialog(dialog_info.auto_trigger_);
		for (auto &content: dialog_info.contents_) {
			dialog->Append(content.texts_, content.avatar_sprite_);
		}
		room->dialogs_.push_back(dialog);

		auto OnTrigger = [dialog_info, dialog, room]()->void {
			dialog->Reset();
			dialog->RegisterKeyEvents();
			room->cur_dialog = dialog;
		};
		// create Trigger
		Trigger* trigger = Trigger::Create(*room, dialog_info.trigger_box_,nullptr, dialog_info.hit_time_remain_);
		trigger->SetOnEnter(OnTrigger);
	}

	for (const auto& door_position : doors_) {
		Door::Create(*room, door_position);
	}

	for (const auto& item_info : item_infos_) {
		float dice = Random::Instance()->Generate();
		if (dice < item_info.probability_) {
			ItemPickUp::Generate(*room, item_info.item_prototype_, item_info.position_, true);
		}
	}

	for (const auto& monster : monsters_) {
		monster.monster_prototype_->Create(*room, monster.initial_pos_, monster.move_radius_, level);
	}

    if (shopInfo_.contain_shop_) {
        room->shop_ = new Shop(shopInfo_.pos_);
        room->shop_->GenerateItems(level);

        auto OnTrigger = [room]()->void {
            room->shop_->RegisterKeyEvents();
        };

        auto OnLeave = [room]()-> void {
            room->shop_->UnregisterKeyEvents();
        };
        // create Trigger
        glm::vec4 trigger_box = glm::vec4(
                shopInfo_.pos_.x,shopInfo_.pos_.y,100,100);
        Trigger* trigger = Trigger::Create(*room, trigger_box, nullptr, 0);
        trigger->SetOnEnter(OnTrigger);
        trigger->SetOnLeave(OnLeave);
    }

	return room;
}

void RoomPrototype::Initialize(Room* room) const
{
	// Monsters
	// for (const auto& monster : monsters_) {
	// 	monster.monster_prototype_->Create(*room, monster.initial_pos_, monster.move_radius_);
	// }
}
