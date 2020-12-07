#pragma once

#include <gamebase/Monster.hpp>
#include <gamebase/Room.hpp>

#include <glm/glm.hpp>

#include <unordered_map>
#include <string>
#include <utility>

// Type object pattern from Game Programming Patterns
// http://gameprogrammingpatterns.com/type-object.html
class MonsterPrototype {
public:
	Monster* Create(Room &room, const glm::vec2 &initial_pos, float move_radius, size_t level);
	Animation* GetAnimation(Mob::AnimationState state) const { return animations_.at(state); }
	ItemPrototype* GenerateItemDrop() const;

	static void LoadConfig(const std::string& monster_list_file);
	static MonsterPrototype* GetMonsterPrototype(const std::string& monster_name) { return &(prototypes_.at(monster_name)); };
	MonsterPrototype() = default;
private:
	static std::unordered_map<std::string, MonsterPrototype> prototypes_;

	std::string name;
	int hp_;
	int attack_;
	int body_attack_;
	int defense_;
	float speed_;
	int exp_;
	int coin_;
	float invulnerable_time_;
	bool is_boss_;
	glm::vec4 bounding_box_;
	std::vector<Attack> attacks_;
	std::unordered_map<Mob::AnimationState, Animation*> animations_;
	json ai_json_;

	using ItemDropInfo = std::pair<ItemPrototype*, float>;
	std::vector<ItemDropInfo> item_drop_infos_;
};
