#pragma once

#include <gamebase/Room.hpp>
#include <gamebase/MonsterPrototype.hpp>
#include <Util.hpp>

#include <glm/glm.hpp>

#include <unordered_map>
#include <string>


class RoomPrototype {
public:
	struct MonsterInfo {
		MonsterPrototype* monster_prototype_;
		glm::vec2 initial_pos_;
		float move_radius_;

		friend void from_json(const json& j, MonsterInfo& monster_info) {
			monster_info.monster_prototype_ = MonsterPrototype::GetMonsterPrototype(j.at("name").get<std::string>());
			j.at("initial_pos").get_to(monster_info.initial_pos_);
			j.at("move_radius").get_to(monster_info.move_radius_);
		}
	};

	struct Trap {
		glm::vec4 bounding_box_;
		int attack_;

		friend void from_json(const json& j, Trap& trap) {
			j.at("bounding_box").get_to(util::AssetSpaceToGameSpace(trap.bounding_box_));
			j.at("attack").get_to(trap.attack_);
		}
	};

	Room* Create() const;
	void Initialize(Room* room) const;

	static void LoadConfig(const std::string& room_list_file);
	static RoomPrototype* GetRoomPrototype(const std::string& room_name) { return &(prototypes_.at(room_name)); };
	RoomPrototype() = default;

private:
	inline static std::unordered_map<std::string, RoomPrototype> prototypes_;

	const Sprite* background_sprite_;
	std::vector<glm::vec4> platforms_;
	std::vector<MonsterInfo> monsters_;
	std::vector<Trap> traps_;
};
