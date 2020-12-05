#include "Monster.hpp"

#include <Util.hpp>
#include <DrawSprites.hpp>
#include <gamebase/MonsterPrototype.hpp>
#include <gamebase/HeadsUpDisplay.hpp>
#include <gamebase/Room.hpp>
#include <engine/Timer.hpp>
#include <gamebase/Player.hpp>

#include <iostream>

Monster::Monster(const MonsterPrototype& monster_prototype, const glm::vec4& bounding_box, int body_attack, Room& room):
Mob(bounding_box, nullptr),
monster_prototype_(monster_prototype),
room_(room)
{
	take_damage_cooldown_ = 0.3f;
	is_monster_ = true;
	collision_aoe_ = AOE::CreateMonsterAOE(room, bounding_box, transform_, body_attack);
}

void Monster::UpdateImpl(float elapsed)
{
	animation_controller_.Update(elapsed);
	ai_->Update(elapsed);
}

void Monster::OnDie()
{
	state_ = State::DYING;
	animation_controller_.PlayAnimation(GetAnimation(AnimationState::DEATH), false);
	collision_aoe_->Destroy();
	TimerManager::Instance().AddTimer(GetAnimation(AnimationState::DEATH)->GetLength(), [&](){
		Destroy();
	});
	// Add coin to player
	player->AddCoin(coin_);
	player->AddExp(exp_);
	// add info to hud to display exp and coin gained
	hud->AddMonsterDieInfoHUD(transform_.position_, coin_, exp_);
}

Animation* Monster::GetAnimation(AnimationState state)
{
	return monster_prototype_.GetAnimation(state);
}

Monster::~Monster() {
	collision_aoe_->Destroy();
}
