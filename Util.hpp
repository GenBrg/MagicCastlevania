#pragma once

#include "Sprite.hpp"
#include "Load.hpp"

#include <glm/glm.hpp>

inline Sprite const *sprite_dunes = nullptr;
inline static constexpr float kGravity { -9.8f };

inline Load< SpriteAtlas > sprites(LoadTagDefault, []() -> SpriteAtlas const * {
	SpriteAtlas const *ret = new SpriteAtlas(data_path("MagicCastlevania"));

	sprite_dunes = &ret->lookup("dunes-ship");

	return ret;
});

namespace util {
	void PrintVec2(const glm::vec2& vec2);
}