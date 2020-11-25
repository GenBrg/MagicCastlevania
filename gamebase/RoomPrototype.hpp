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
			monster_info.initial_pos_ = util::AssetSpaceToGameSpace(j.at("initial_pos").get<glm::vec2>());
			j.at("move_radius").get_to(monster_info.move_radius_);
		}
	};

	struct Trap {
		glm::vec4 bounding_box_;
		int attack_;

		friend void from_json(const json &j, Trap &trap) {
			trap.bounding_box_ = util::AssetSpaceToGameSpace(j.at("bounding_box").get<glm::vec4>());
			j.at("attack").get_to(trap.attack_);
		}
	};

	struct DialogContent {
		std::string texts_;
		std::string avatar_sprite_;

		friend void from_json(const json &j, DialogContent &dialog_content) {
			j.at("texts").get_to(dialog_content.texts_);
			j.at("avatar_sprite").get_to(dialog_content.avatar_sprite_);
		}
	};

	struct DialogInfo {
		glm::vec4 trigger_box_;
		int hit_time_remain_;
		float interval_between_hit_;
		bool auto_trigger_;
		std::vector<DialogContent> contents_;

		friend void from_json(const json &j, DialogInfo &dialog_info) {
			dialog_info.trigger_box_ = util::AssetSpaceToGameSpace(j.at("trigger_box").get<glm::vec4>());
			j.at("hit_time_remain").get_to(dialog_info.hit_time_remain_);
			j.at("interval_between_hit").get_to(dialog_info.interval_between_hit_);
			j.at("auto_trigger").get_to(dialog_info.auto_trigger_);
			for (const auto &content_json : j.at("contents")) {
				dialog_info.contents_.push_back(content_json.get<DialogContent>());
			}
		}
	};

	Room *Create() const;

	void Initialize(Room *room) const;

	static void LoadConfig(const std::string &room_list_file);

	static RoomPrototype *GetRoomPrototype(const std::string &room_name) { return &(prototypes_.at(room_name)); };
	static size_t GetRoomPrototypeNum() { return prototypes_.size(); }

	RoomPrototype() = default;

private:
	static std::unordered_map<std::string, RoomPrototype> prototypes_;

	const Sprite *background_sprite_;
	std::vector<glm::vec4> platforms_;
	std::vector<MonsterInfo> monsters_;
	std::vector<Trap> traps_;
	std::vector<DialogInfo> dialog_infos_;
	std::vector<glm::vec2> doors_;
};
