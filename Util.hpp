#pragma once

#include <Sprite.hpp>
#include <Load.hpp>
#include <data_path.hpp>
#include <Sound.hpp>

#include <nlohmann/json.hpp>

#include <glm/glm.hpp>

#define INIT_WINDOW_W 960
#define INIT_WINDOW_H 541
#define SWITCH_ROOM_TRANSITION 1.1f
#define ON_DIE_TRANSITION 3.0f

//#define _DEBUG

inline Sprite const* sprite_bg = nullptr;
inline Sprite const* sprite_ghost = nullptr;

inline glm::vec2 VIEW_MIN = glm::vec2(0, 0);
inline glm::vec2 VIEW_MAX = glm::vec2(960, 541);

inline static constexpr float kGravity{ -980.0f };

extern class Player* player;
extern class HeadsUpDisplay* hud;

using json = nlohmann::json;

inline Load< SpriteAtlas > sprites(LoadTagEarly, []() -> SpriteAtlas* const {
	SpriteAtlas *ret = new SpriteAtlas();
	ret->LoadSprites(data_path("MagicCastlevania"));
	ret->LoadSprites(data_path("Monsters"));
	return ret;
});

inline std::unordered_map<std::string, Sound::Sample*> sound_samples;

namespace glm {
	void from_json(const json& j, vec2& p);
	void from_json(const json& j, vec3& p);
	void from_json(const json& j, vec4& p);
}

namespace util {
	void PrintVec2(const glm::vec2& vec2);
	void PrintVec4(const glm::vec4& vec4);
	void PrintMat3(const glm::mat3& mat3);
	void PrintMat4(const glm::mat4& mat4);

	glm::vec4 AssetSpaceToGameSpace(const glm::vec4& bounding_box);
	glm::vec2 AssetSpaceToGameSpace(const glm::vec2& bounding_box);
}