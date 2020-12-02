#include "Attack.hpp"

#include <Util.hpp>

bool Attack::Execute(Room& room, int attack, Transform2D& transform, bool is_monster)
{
	return guard_(cooldown_, [&](){
		aoe_prototype_->Create(room, attack, &transform, is_monster, attach_to_entity_);
	});
}

void from_json(const json& j, Attack& attack)
{
	attack.mob_animation_ = Animation::GetAnimation(j.at("mob_animation"));
	attack.aoe_prototype_ = AOEPrototype::GetAOEPrototype(j.at("aoe_prototype"));
	attack.cooldown_ = j.at("cooldown");
	attack.attach_to_entity_ = j.at("attach_to_entity");
	if (j.contains("icon")) {
		json icon_json = j.at("icon");
		if (!icon_json.is_null()) {
			attack.icon_sprite_ = &(sprites->lookup(icon_json.get<std::string>()));
		}
	} else {
		attack.icon_sprite_ = nullptr;
	}
}
