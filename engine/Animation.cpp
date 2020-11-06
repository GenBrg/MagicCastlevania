#include "Animation.hpp"
#include "../Util.hpp"

#include <stdexcept>
#include <algorithm>
#include <iostream>

std::unordered_map<std::string, AnimationController::Animation> AnimationController::animation_collection;

void AnimationController::PlayAnimation(const std::string& animation_name, float interval, bool loop)
{
	auto it = animation_collection.find(animation_name);
	if (it == animation_collection.end()) {
		throw std::runtime_error("Cannot find animation " + animation_name);
	}

	animation_ = &((*it).second);
	playing_ = true;
	loop_ = loop;
	interval_ = interval;
	time_since_begin_ = 0.0f;
	length_ = interval * animation_->size();
}

void AnimationController::Update(float elapsed)
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

void AnimationController::Draw(DrawSprites& draw) const
{
	if (current_sprite_) {
		draw.draw(*current_sprite_, transform_);
	}
}

void AnimationController::LoadAnimation(const std::string& animation_name, int sprite_num)
{
	std::cout << "Load animation: player_" << animation_name << std::endl;

	if (animation_collection.count(animation_name) > 0) {
		throw std::runtime_error("Duplicate animation name: " + animation_name);
	}

	Animation& animation = animation_collection[animation_name];

	for (int i = 1; i <= sprite_num; ++i) {
		animation.emplace_back(&(sprites->lookup(animation_name + "_" + std::to_string(i))));
	}
}