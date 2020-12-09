#include "Attack.hpp"

#include <engine/Random.hpp>
#include <gamebase/Player.hpp>
#include <Util.hpp>

#include <iostream>

bool Attack::Execute(Room& room, int attack, Transform2D& transform, bool is_monster)
{
	return guard_(cooldown_, [&](){
		attack = static_cast<int>(base_attack_per_level_ * player->GetLevel() + attack * attack_modifier_);
		std::cout << "attack value: " << attack << std::endl;
		aoe_prototype_->Create(room, attack, &transform, is_monster, attach_to_entity_);
	});
}

void from_json(const json& j, Attack& attack)
{
	attack.mob_animation_ = Animation::GetAnimation(j.at("mob_animation"));
	attack.aoe_prototype_ = AOEPrototype::GetAOEPrototype(j.at("aoe_prototype"));
	attack.cooldown_ = j.at("cooldown");
	attack.attach_to_entity_ = j.at("attach_to_entity");
	if (j.contains("sound_effects")) {
        for (const auto& sound_effect : j.at("sound_effects")) {
            attack.sound_effect_.push_back(sound_effect);
        }
	}
	if (j.contains("icon")) {
		json icon_json = j.at("icon");
		if (!icon_json.is_null()) {
			attack.icon_sprite_ = &(sprites->lookup(icon_json.get<std::string>()));
		}
	} else {
		attack.icon_sprite_ = nullptr;
	}
	if (j.contains("base_attack_per_level")) {
		attack.base_attack_per_level_ = j.at("base_attack_per_level").get<int>();
	}
	if (j.contains("attack_modifier")) {
		attack.attack_modifier_ = j.at("attack_modifier").get<float>();
	}
}

void Attack::PlayAttackSound() const {
    if (!sound_effect_.empty()) {
        int sound_idx = static_cast<int>(Random::Instance()->Generate() * sound_effect_.size());
        Sound::play(*sound_samples.at(sound_effect_[sound_idx]));
    }
}
