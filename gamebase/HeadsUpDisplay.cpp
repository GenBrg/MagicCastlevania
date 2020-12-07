#include "HeadsUpDisplay.hpp"
#include "../Util.hpp"
#include "../Load.hpp"
#include <iostream>
#include <stack>
#define MONSTER_DIE_HUD_LAST 0.8f
#define ITEM_ANIMATION_TRANSITION 0.05f


const Sprite* GetCoinSprite(float elapsed) {
    int idx = (int)(elapsed / ITEM_ANIMATION_TRANSITION);
    return &sprites->lookup("coin_" + std::to_string(idx % 5 + 1));
}

const Sprite* GetExpSprite(float elapsed) {
    int idx = (int)(elapsed / ITEM_ANIMATION_TRANSITION);
    return &sprites->lookup("arrow_up_" + std::to_string(idx % 5 + 1));
}

void DrawDigits(DrawSprites& draw_sprite, glm::vec2 pos, int val, glm::u8vec4 tint) {
    std::stack<int> digits;
    while(val > 0) {
        digits.push(val%10);
        val = val / 10;
    }

    Transform2D transform(nullptr);
    transform.position_ = pos;

    while(!digits.empty()) {
        int digit = digits.top();
        digits.pop();
        auto digit_sprite = sprites->lookup("digit_" + std::to_string(digit));
        draw_sprite.draw(digit_sprite, transform, tint);
        transform.position_.x += digit_sprite.size_px.x;
    }
}


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
	skill_transform.position_ = hp_bar_transform_.position_ + glm::vec2(-38.0f, -40.0f);
	for(auto attack: player->GetAttackInfo()) {
		if (attack.GetIconSprite() == nullptr) {
			continue;
		}
        skill_transform.position_ += glm::vec2(38.0f, 0.0f);
		auto alpha = (uint8_t)(0xff * (1.0f - attack.GetCoolDownLeftPercent()));
		draw_sprite.draw(*(attack.GetIconSprite()), skill_transform, glm::u8vec4(0xff, 0xff, 0xff, alpha));
	}

	for (auto die_info: monster_die_info_queue_) {
	    // render one die info for one monster
	    auto die_info_transform = Transform2D(nullptr);
	    die_info_transform.position_ = die_info.pos_;
        auto alpha = (uint8_t)(0xff * (1.0f - die_info.elapsed_/MONSTER_DIE_HUD_LAST));
        auto tint = glm::u8vec4(0xff, 0xff, 0xff, alpha);
        // draw coin icon
        draw_sprite.draw(*GetCoinSprite(die_info.elapsed_), die_info_transform, tint);
        // draw coin number
        glm::vec2 coin_digit_pos = die_info_transform.position_ + glm::vec2(sprites->lookup("coin_1").size_px.x + 5, 0);
        DrawDigits(draw_sprite, coin_digit_pos, die_info.coin_, tint);

        // draw exp icon
        die_info_transform.position_ += glm::vec2(0, sprites->lookup("coin_1").size_px.y);
        draw_sprite.draw(*GetExpSprite(die_info.elapsed_), die_info_transform, tint);

        // draw exp number
        glm::vec2 exp_digit_pos = die_info_transform.position_ + glm::vec2(sprites->lookup("arrow_up_1").size_px.x + 5, 0);
        DrawDigits(draw_sprite, exp_digit_pos, die_info.exp_, tint);
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
