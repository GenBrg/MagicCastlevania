#include "HeadsUpDisplay.hpp"
#include "../Util.hpp"
#include "../Load.hpp"
#include <iostream>

#define MONSTER_DIE_HUD_LAST 1.0f

HeadsUpDisplay::HeadsUpDisplay() : 
hp_bar_transform_(nullptr)
{
	hp_bar_transform_.position_ = glm::vec2(9.0f, 512.0f);
}
void HeadsUpDisplay::Draw(DrawSprites& draw_sprite) const {
	draw_sprite.draw(sprites->lookup("hp_bar"), hp_bar_transform_);
	int hp_to_draw = player->GetHp();
	if (hp_to_draw <= 0) {
		return;
	}
	Transform2D hp_curr_transform = Transform2D(nullptr);
	hp_curr_transform.position_ = hp_bar_transform_.position_ + glm::vec2(3.0f,4.0f);
	draw_sprite.draw(sprites->lookup("hp_corner1"), hp_curr_transform);
	hp_curr_transform.position_ += glm::vec2(4.0f, 0.0f);
	hp_to_draw -= 3;
	if (hp_to_draw < 3) {
		return;
	}
	for (; hp_to_draw >= 3; hp_to_draw--) {
		draw_sprite.draw(sprites->lookup("hp_point"), hp_curr_transform);
		hp_curr_transform.position_ += glm::vec2(1.0f, 0.0f);
	}
	draw_sprite.draw(sprites->lookup("hp_corner2"), hp_curr_transform);

	auto skill_transform = Transform2D(nullptr);
	skill_transform.position_ = hp_bar_transform_.position_ + glm::vec2(30.0f, 0.0f);
	for(auto attack: player->GetAttackInfo()) {
		if (attack.GetIconSprite() == nullptr) {
			continue;
		}
        skill_transform.position_ += glm::vec2(0.0f, -80.0f);
		auto alpha = (uint8_t)(0xff * (1.0f - attack.GetCoolDownLeftPercent()));
		draw_sprite.draw(*(attack.GetIconSprite()), skill_transform, glm::u8vec4(0xff, 0xff, 0xff, alpha));
	}

	for (auto die_info: monster_die_info_queue_) {
	    // render one die info for one monster
	    auto die_info_transform = Transform2D(nullptr);
	    die_info_transform.position_ = die_info.pos_;
        auto alpha = (uint8_t)(0xff * (1.0f - die_info.elapsed_/MONSTER_DIE_HUD_LAST));
        draw_sprite.draw(sprites->lookup("avatar_mage"), die_info_transform,
                         glm::u8vec4(0xff, 0xff, 0xff, alpha));
	}
}

void HeadsUpDisplay::AddMonsterDieInfoHUD(glm::vec2 pos, int coin, int exp) {
    MonsterDieInfo monster_die_info{};
    monster_die_info.coin_ = coin;
    monster_die_info.exp_ = exp;
    monster_die_info.pos_ = pos + glm::vec2(0.0f, 40.0f); // a bit higher
    monster_die_info.elapsed_ = 0.0f;
    monster_die_info_queue_.push_back(monster_die_info);
}

void HeadsUpDisplay::Update(float elapsed) {
    while (!monster_die_info_queue_.empty() && monster_die_info_queue_.front().elapsed_ + elapsed > MONSTER_DIE_HUD_LAST) {
        monster_die_info_queue_.pop_front();
    }
    for(auto& die_info: monster_die_info_queue_) {
        die_info.elapsed_ += elapsed;
    }
}
