#pragma once

#include "../Sprite.hpp"
#include "../DrawSprites.hpp"
#include "Transform2D.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <iostream>

using Animation = std::vector<const Sprite*>;
// TODO change lookup system from string look up to template based enum look up.
template <typename AnimationState>
class AnimationController {
public: 
	AnimationController(Transform2D* parent_transform) :
	transform_(parent_transform)
	{}

	void PlayAnimation(AnimationState state, float interval, bool loop, bool force_replay = true)
	{
		if (!force_replay && animation_state_ == state) {
			return;
		}

		auto it = animation_collection.find(state);
		if (it == animation_collection.end()) {
			throw std::runtime_error("Cannot find animation " + std::to_string(static_cast<uint32_t>(state)));
		}

		animation_ = &((*it).second);
		playing_ = true;
		loop_ = loop;
		interval_ = interval;
		time_since_begin_ = 0.0f;
		animation_state_ = state;
		length_ = interval * animation_->size();
	}

	void Update(float elapsed)
	{
		if (!playing_) {
			return;
		}

		time_since_begin_ += elapsed;

		// Wrap around
		if (time_since_begin_ >= length_) {
			if (!loop_) {
				playing_ = false;
				current_sprite_ = animation_->back();
				return;
			} else {
				time_since_begin_ -= length_;
			}
		}

		int sprite_idx = static_cast<int>(time_since_begin_ / interval_);

		// Avoid potential overflow
		sprite_idx = std::clamp(sprite_idx, 0, static_cast<int>(animation_->size()));
		current_sprite_ = (*animation_)[sprite_idx];
	}

	void Draw(DrawSprites& draw) const
	{
		if (current_sprite_) {
			draw.draw(*current_sprite_, transform_);
		}
	}

	float GetLength() const { return length_; }

	AnimationState GetAnimationState() const { return animation_state_; }

	static void LoadAnimation(AnimationState state, const std::string& animation_name, int sprite_num)
	{
		if (animation_collection.count(state) > 0) {
			throw std::runtime_error("Duplicate animation name: " + animation_name);
		}

		std::cout << "Load animation: " << animation_name << std::endl;

		Animation& animation = animation_collection[state];

		for (int i = 1; i <= sprite_num; ++i) {
			animation.emplace_back(&(sprites->lookup(animation_name + "_" + std::to_string(i))));
		}
	}

private:
	inline static std::unordered_map<AnimationState, Animation> animation_collection;

	Transform2D transform_;
	Animation* animation_ { nullptr };
	AnimationState animation_state_;
	const Sprite* current_sprite_ { nullptr };
	float interval_;
	float time_since_begin_ { 0.0f };
	float length_;

	bool playing_ { false };
	bool loop_ { false };
};
