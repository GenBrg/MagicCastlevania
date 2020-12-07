#include "MonsterPrototype.hpp"

#include <gamebase/MonsterAI.hpp>
#include <gamebase/ItemPrototype.hpp>
#include <data_path.hpp>
#include <Util.hpp>
#include <main_play.hpp>
#include <engine/Random.hpp>

#include <fstream>
#include <stdexcept>
#include <iostream>

std::unordered_map<std::string, MonsterPrototype> MonsterPrototype::prototypes_;

Monster *MonsterPrototype::Create(Room &room, const glm::vec2 &initial_pos, float move_radius)
{
	int level = main_play->GetLevel();
	Monster *monster = new Monster(*this, bounding_box_, body_attack_, room);
	monster->move_radius_ = move_radius;
	monster->central_pos_ = initial_pos;
	monster->transform_.position_ = initial_pos;
	if (is_boss_)
	{
		monster->hp_ = hp_;
		monster->attack_ = attack_;
		monster->defense_ = defense_;
		monster->exp_ = exp_;
		monster->coin_ = coin_;
	}
	else
	{
		monster->hp_ = static_cast<int>((1 + 0.5f * (level - 1)) * hp_);
		monster->attack_ = static_cast<int>((1 + 0.3f * (level - 1)) * attack_);
		monster->defense_ = static_cast<int>((1 + 0.3f * (level - 1)) * defense_);
		monster->exp_ = static_cast<int>((1 + 0.5f * (level - 1)) * exp_);
		monster->coin_ = static_cast<int>((1 + 0.5f * (level - 1)) * coin_);
	}

	monster->take_damage_cooldown_ = invulnerable_time_;
	monster->speed_ = speed_;
	monster->animation_controller_.PlayAnimation(monster->GetAnimation(Mob::AnimationState::WALK), true, true);
	monster->skills_ = attacks_;
	monster->is_boss = is_boss_;
	monster->ai_ = IMonsterAI::GetMonsterAI(monster, ai_json_);
	room.AddMonster(monster);
	return monster;
}

void MonsterPrototype::LoadConfig(const std::string &monster_list_file)
{
	std::ifstream f(monster_list_file);
	if (!f.is_open())
	{
		throw std::runtime_error("Can not load monster list file: " + monster_list_file);
	}

	std::string monster_name;
	while (f >> monster_name)
	{
		if (prototypes_.count(monster_name) > 0)
		{
			throw std::runtime_error("Duplicate monster name: " + monster_name);
		}

		std::string config_file_name = data_path("monsters/" + monster_name + ".json");
		std::ifstream monster_file(config_file_name);
		if (!monster_file.is_open())
		{
			throw std::runtime_error("Can not load monster file: " + config_file_name);
		}
		json j;
		monster_file >> j;
		std::cout << "Load monster prototype " << monster_name << std::endl;

		prototypes_.emplace(monster_name, MonsterPrototype());
		MonsterPrototype &monster_prototype = prototypes_[monster_name];
		monster_prototype.hp_ = j.at("hp");
		monster_prototype.attack_ = j.at("attack");
		monster_prototype.body_attack_ = j.at("body_attack");
		monster_prototype.defense_ = j.at("defense");
		monster_prototype.speed_ = j.at("speed");
		monster_prototype.exp_ = j.at("exp");
		monster_prototype.coin_ = j.at("coin");
		monster_prototype.invulnerable_time_ = j.at("invulnerable_time");
		monster_prototype.bounding_box_ = j.at("bounding_box").get<glm::vec4>();
		monster_prototype.ai_json_ = j.at("ai");
		monster_prototype.is_boss_ = j.at("is_boss");
		for (const auto &attack_json : j.at("skills"))
		{
			monster_prototype.attacks_.push_back(attack_json.get<Attack>());
		}
		float acc_prob = 0.0f;
		for (const auto &[key, val] : j.at("item").items())
		{
			acc_prob += val.get<float>();
			monster_prototype.item_drop_infos_.emplace_back(ItemPrototype::GetPrototype(key), acc_prob);
		}
		assert(acc_prob <= 1.0f);

		// Load animations
		for (const auto &[animation_name, animation_state_name] : Mob::kAnimationNameStateMap)
		{
			monster_prototype.animations_[animation_state_name] = Animation::GetAnimation(monster_name + "_" + animation_name);
		}
	}
}

ItemPrototype *MonsterPrototype::GenerateItemDrop() const
{
	float dice = Random::Instance()->Generate();
	for (const auto &[item, acc_prob] : item_drop_infos_)
	{
		if (dice < acc_prob)
		{
			return item;
		}
	}

	return nullptr;
}
