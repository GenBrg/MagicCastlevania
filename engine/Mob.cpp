#include "Mob.hpp"

#include <engine/Timer.hpp>

#include <iostream>

const std::unordered_map<std::string, Mob::AnimationState> Mob::kAnimationNameStateMap 
{
	{ "idle", Mob::AnimationState::STILL },
	{ "walk", Mob::AnimationState::WALK },
	{ "jump", Mob::AnimationState::JUMP },
	{ "fall", Mob::AnimationState::FALL },
	{ "hurt", Mob::AnimationState::HURT },
	{ "death", Mob::AnimationState::DEATH }
};

void Mob::TakeDamage(int attack)
{
	if (state_ != State::MOVING && state_ != State::ATTACKING)
	{
		return;
	}

	take_damage_guard_(take_damage_cooldown_, [&]() {
		PlayTakeDamageSound();

		hp_ -= GetDamagePoint(attack);

		if (hp_ <= 0)
		{
			OnDie();
		}
		else
		{
			animation_controller_.PlayAnimation(GetAnimation(AnimationState::HURT), false);
			state_ = State::TAKING_DAMAGE;
			++pending_callbacks_;
			TimerManager::Instance().AddTimer(GetAnimation(AnimationState::HURT)->GetLength(), [&]() {
				--pending_callbacks_;
				if (state_ == State::TAKING_DAMAGE)
				{
					state_ = State::MOVING;
					animation_controller_.PlayAnimation(GetAnimation(AnimationState::STILL), true, true);
				}
			});
		}
	});
}

void Mob::PerformAttack(Room& room, Attack& attack)
{
	if (state_ != State::MOVING)
	{
		return;
	}

	Animation *animation = attack.GetAnimation();
	if (attack.Execute(room, GetAttackPoint(), transform_, is_monster_))
	{
		PlayAttackSound();
		animation_controller_.PlayAnimation(animation, false);
		state_ = State::ATTACKING;

		pending_callbacks_++;
		TimerManager::Instance().AddTimer(animation->GetLength(), [&]() {
			pending_callbacks_--;
			if (state_ == State::ATTACKING)
			{
				state_ = State::MOVING;
				animation_controller_.PlayAnimation(GetAnimation(AnimationState::STILL), true, true);
			}
		});
	}
}

Mob::Mob(const glm::vec4& bounding_box, Transform2D* transform) :
Entity(bounding_box, transform),
animation_controller_(&transform_)
{}

bool Mob::IsDestroyed() const
{
	return destroyed_ && pending_callbacks_ == 0;
}
