#pragma once

#include "../engine/Transform2D.hpp"
#include "../DrawSprites.hpp"
#include "Player.hpp"

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>


class HeadsUpDisplay {
private:
	Transform2D hp_bar_transform_;
public:
	void Draw(DrawSprites& draw_sprite) const;
	HeadsUpDisplay();
};