#pragma once

#include "Sprite.hpp"
#include "Load.hpp"
#include "data_path.hpp"

#include <glm/glm.hpp>

inline Sprite const *sprite_bg = nullptr;
inline Sprite const *sprite_ghost = nullptr;

inline static constexpr float kGravity { -100.0f };

inline Load< SpriteAtlas > sprites(LoadTagDefault, []() -> SpriteAtlas const * {
	SpriteAtlas const *ret = new SpriteAtlas(data_path("MagicCastlevania"));

	sprite_bg = &ret->lookup("bg");
	sprite_ghost = &ret->lookup("ghost_idle_1");

	return ret;
});

namespace util {
	void PrintVec2(const glm::vec2& vec2);
	void PrintMat3(const glm::mat3& mat3);
}