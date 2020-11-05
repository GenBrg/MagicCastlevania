#pragma once

#include "Sprite.hpp"
#include "Load.hpp"
#include "data_path.hpp"

#include <glm/glm.hpp>

inline Sprite const *sprite_dunes = nullptr;
inline Sprite const *sprite_oasis_traveller = nullptr;
inline static constexpr float kGravity { -100.0f };

inline Load< SpriteAtlas > sprites(LoadTagDefault, []() -> SpriteAtlas const * {
	SpriteAtlas const *ret = new SpriteAtlas(data_path("MagicCastlevania"));

	sprite_dunes = &ret->lookup("dunes-ship");
	sprite_oasis_traveller = &ret->lookup("oasis-traveller");

	return ret;
});

namespace util {
	void PrintVec2(const glm::vec2& vec2);
	void PrintMat3(const glm::mat3& mat3);
}