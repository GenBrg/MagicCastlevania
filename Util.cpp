#include "Util.hpp"

#include <engine/Animation.hpp>
#include <engine/AOEPrototype.hpp>
#include <gamebase/Player.hpp>
#include <gamebase/MonsterPrototype.hpp>
#include <gamebase/RoomPrototype.hpp>
#include <gamebase/PotionPrototype.hpp>
#include <gamebase/EquipmentPrototype.hpp>
#include <gamebase/HeadsUpDisplay.hpp>
#include <gamebase/DoorKey.hpp>
#include <gamebase/Door.hpp>

#include <iostream>
#include <fstream>
#include <stdexcept>

Player* player;
HeadsUpDisplay* hud;

Load<void> load_everything(LoadTagDefault, [](){
	Animation::LoadAnimation(data_path("animations.json"));
	AOEPrototype::LoadConfig(data_path("aoe_prototypes.json"));
	PotionPrototype::LoadConfig(data_path("items/potions.json"));
	EquipmentPrototype::LoadConfig(data_path("items/equipments.json"));
	MonsterPrototype::LoadConfig(data_path("monsters/monster.list"));
	RoomPrototype::LoadConfig(data_path("rooms/room.list"));
	DoorKey::sprite_ = &(sprites->lookup("key"));
	Door::lock_sprite_ = &(sprites->lookup("lock"));
});

Load<void> load_sounds(LoadTagDefault, [](){
	std::string bgm_path = data_path("sound/bgm/opus/");
	std::ifstream f(bgm_path + "sound.list");
	std::string sound_file_name;
	if (!f.is_open()) {
		throw std::runtime_error("Can not load bgm list file");
	}

	while (f >> sound_file_name) {
		sound_samples[sound_file_name] = new Sound::Sample(bgm_path + sound_file_name + ".opus");
	}

	f.close();
	f.clear();

	std::string sfx_path = data_path("sound/effect/opus/");
	f.open(sfx_path + "sound.list");
	if (!f.is_open()) {
		throw std::runtime_error("Can not load sfx list file");
	}

	while (f >> sound_file_name) {
		sound_samples[sound_file_name] = new Sound::Sample(sfx_path + sound_file_name + ".opus");
	}
});

namespace glm {
	void from_json(const json& j, vec2& p)
	{
		for (int i = 0; i < 2; ++i)
			j.at(i).get_to(p[i]);
	}
	void from_json(const json& j, vec3& p)
	{
		for (int i = 0; i < 3; ++i)
			j.at(i).get_to(p[i]);
	}
	void from_json(const json& j, vec4& p)
	{
		for (int i = 0; i < 4; ++i)
			j.at(i).get_to(p[i]);
	}
}

namespace util
{
	void PrintVec2(const glm::vec2& vec2)
	{
		std::cout << vec2.x << " " << vec2.y << std::endl;
	}

	void PrintVec4(const glm::vec4& vec4)
	{
		std::cout << vec4.x << " " << vec4.y << " " << vec4.z << " " << vec4.w << std::endl;
	}

	void PrintMat3(const glm::mat3& mat3)
	{
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				std::cout << mat3[j][i] << " ";
			}
			std::cout << std::endl;
		}
	}

	void PrintMat4(const glm::mat4& mat4)
	{
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				std::cout << mat4[j][i] << " ";
			}
			std::cout << std::endl;
		}
	}

	glm::vec4 AssetSpaceToGameSpace(const glm::vec4& bounding_box)
	{
		return glm::vec4(bounding_box[0], VIEW_MAX.y - bounding_box[1] - bounding_box[3],
	 					 bounding_box[0] + bounding_box[2], VIEW_MAX.y - bounding_box[1]);
	}

	glm::vec2 AssetSpaceToGameSpace(const glm::vec2& bounding_box)
	{
		return glm::vec2(bounding_box[0], VIEW_MAX.y - bounding_box[1]);
	}
} // namespace util
