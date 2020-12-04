#pragma once

#include "../engine/Transform2D.hpp"
#include "../DrawSprites.hpp"
#include "Player.hpp"

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <queue>


class HeadsUpDisplay {
private:
	Transform2D hp_bar_transform_;

	//
	struct MonsterDieInfo {
	    int coin_;
	    int exp_;
	    float elapsed_;
	    glm::vec2 pos_;
	};
	std::deque<MonsterDieInfo> monster_die_info_queue_;
public:
    void AddMonsterDieInfoHUD(glm::vec2 pos, int coin, int exp);
	void Draw(DrawSprites& draw_sprite) const;
	void Update(float elapsed);
	HeadsUpDisplay();
};