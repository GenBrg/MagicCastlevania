#include "AOEPrototype.hpp"

#include <gamebase/Room.hpp>
#include <engine/AOE.hpp>
#include <engine/Animation.hpp>
#include <data_path.hpp>
#include <Util.hpp>

#include <stdexcept>
#include <fstream>
#include <sstream>

void AOEPrototype::LoadConfig(const std::string& config_file)
{
	std::ifstream f(config_file);
	if (!f.is_open()) {
		throw std::runtime_error("Can not open AOEPrototype file: " + config_file);
	}

	json j;
	f >> j;

	for (const auto& aoe_prototype_json : j) {
		std::string name = aoe_prototype_json.at("name");
		std::cout << "Load AOE prototype " << name << std::endl;

		glm::vec4 bounding_box = aoe_prototype_json.at("bounding_box").get<glm::vec4>();
		glm::vec2 velocity = aoe_prototype_json.at("velocity").get<glm::vec2>();
		float duration = aoe_prototype_json.at("duration");
		glm::vec2 initial_offset = aoe_prototype_json.at("initial_offset");
		bool penetrate = aoe_prototype_json.at("penetrate");

		Animation* animation = nullptr;
		auto anim = aoe_prototype_json.at("animation");
		if (!anim.is_null()) {
			animation = Animation::GetAnimation(anim.get<std::string>());
		}

		prototypes_.emplace(name, AOEPrototype(bounding_box, animation, velocity, duration, initial_offset, penetrate));
	}
}

AOE* AOEPrototype::Create(Room& room, int attack, Transform2D* parent_transform, bool is_monster, bool attach_to_entity)
{
	if (!attach_to_entity) {
		if (parent_transform) {
			initial_offset_ += parent_transform->scale_.x * parent_transform->position_;
		}
		parent_transform = nullptr;
	}

	AOE* aoe = new AOE(bounding_box_, animation_, velocity_, duration_, attack, initial_offset_, penetrate_, parent_transform);
	if (is_monster) {
		room.AddMonsterAOE(aoe);
	} else {
		room.AddPlayerAOE(aoe);
	}
	return aoe;
}

std::string AOEPrototype::to_string() const
{
	std::ostringstream oss;
	oss << duration_ << " " << penetrate_;
	return oss.str();
}
