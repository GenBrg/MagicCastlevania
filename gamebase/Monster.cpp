#include "../Util.hpp"
#include "../DrawSprites.hpp"
#include "Monster.hpp"
#include "Room.hpp"

#include <iostream>

Monster::Monster(const MonsterPrototype& monster_prototype, const glm::vec4& bounding_box, int body_attack, Room& room):
monster_prototype_(monster_prototype),
Mob(bounding_box, nullptr)
{
	collision_aoe_ = AOE::CreateMonsterAOE(room, bounding_box, transform_, body_attack);
}

void Monster::UpdateImpl(float elapsed)
{
	transform_.position_.x += speed_ * elapsed * (float)mov_direction_;
	if(transform_.position_.x > central_pos_.x + move_radius_ ||
		transform_.position_.x < central_pos_.x - move_radius_) {
		transform_.position_.x -= speed_ * elapsed * (float)mov_direction_;
		mov_direction_ *= -1;
	}
}

void Monster::OnDie()
{
	Destroy();
	collision_aoe_->Destroy();
}
