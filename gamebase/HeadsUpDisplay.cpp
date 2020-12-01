#include "HeadsUpDisplay.hpp"
#include "../Util.hpp"
#include "../Load.hpp"
#include <iostream>
HeadsUpDisplay::HeadsUpDisplay(Player* player) : 
hp_bar_transform_(nullptr)
{
	hp_bar_transform_.position_ = glm::vec2(9.0f, 512.0f);
	player_ = player;
}
void HeadsUpDisplay::Draw(DrawSprites& draw_sprite) const {
	draw_sprite.draw(sprites->lookup("hp_bar"), hp_bar_transform_);
	int hp_to_draw = player_->GetHp();
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

	Transform2D skill_transform = Transform2D(nullptr);
	skill_transform.position_ = hp_bar_transform_.position_ + glm::vec2(30.0f, 0.0f);
	for(auto attack: player_->GetAttackInfo()) {
		if (attack.GetIconSprite() == nullptr) {
			continue;
		}
        skill_transform.position_ += glm::vec2(0.0f, -80.0f);
		if(attack.GetCoolDownLeftPercent() < 1e-3) {
			draw_sprite.draw(*(attack.GetIconSprite()), skill_transform);
		}
	}
}