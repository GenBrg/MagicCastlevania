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
	collision_aoe_ = nullptr;
	pending_callbacks_++;
	TimerManager::Instance().AddTimer(GetAnimation(AnimationState::DEATH)->GetLength(), [&](){
		pending_callbacks_--;
		Destroy();
	});
	// Add coin to player
	player->AddCoin(coin_);
	player->AddExp(exp_);
	// add info to hud to display exp and coin gained
	hud->AddMonsterDieInfoHUD(transform_.position_, coin_, exp_);

	// Drop item
	if (ItemPrototype* drop_item = monster_prototype_.GenerateItemDrop()) {
		ItemPickUp::Generate(room_, drop_item, transform_.position_);
	}
}

Animation* Monster::GetAnimation(AnimationState state)
{
	return monster_prototype_.GetAnimation(state);
}

Monster::~Monster() {
	if (collision_aoe_) {
		collision_aoe_->Destroy();
		collision_aoe_ = nullptr;
	}
	delete ai_;
}

void Monster::Destroy()
{
	destroyed_ = true;
	if (collision_aoe_) {
		collision_aoe_->Destroy();
		collision_aoe_ = nullptr;
	}
}

void Monster::PlayTakeDamageSound()
{
	Sound::play(*sound_samples["monster_being_attack"]);
}
