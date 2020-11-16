#include "Animation.hpp"
#include "../Util.hpp"

#include <fstream>
#include <stdexcept>

void AnimationController::PlayAnimation(Animation* animation, bool loop, bool force_replay)
{
	if (!force_replay && animation_ == animation) {
		return;
	}

	animation_ = animation;
	playing_ = true;
	loop_ = loop;
	time_since_begin_ = 0.0f;
	if (animation) {
		length_ = animation->GetLength();
	}
}

void AnimationController::Update(float elapsed)
{
	if (!playing_ || !animation_) {
		return;
	}

	time_since_begin_ += elapsed;

	// Wrap around
	if (time_since_begin_ >= length_) {
		if (!loop_) {
			playing_ = false;
			current_sprite_ = animation_->sprites_.back();
			return;
		} else {
			time_since_begin_ -= length_;
		}
	}

	int sprite_idx = static_cast<int>(time_since_begin_ / animation_->interval_);

	// Avoid potential overflow
	sprite_idx = std::clamp(sprite_idx, 0, static_cast<int>(animation_->sprites_.size()));
	current_sprite_ = (animation_->sprites_)[sprite_idx];
}

void AnimationController::Draw(DrawSprites& draw) const
{
	if (current_sprite_) {
		draw.draw(*current_sprite_, transform_);
	}
}

void Animation::LoadAnimation(const std::string& config_file_name)
{
	std::ifstream f(config_file_name);
	if (!f.is_open()) {
		throw std::runtime_error("Cannot load animation: " + config_file_name);
	}

	json j;
	f >> j;

	for (const auto& animation_json : j) {
		std::string animation_name = animation_json.at("name");
		int sprite_num = animation_json.at("sprite_num");
		float interval = animation_json.at("interval");

		if (animation_collection.count(animation_name) > 0) {
			throw std::runtime_error("Duplicate animation name: " + animation_name);
		}

		std::cout << "Load animation: " << animation_name << std::endl;

		animation_collection.emplace(animation_name, Animation(animation_name, sprite_num, interval));
	}
}

Animation::Animation(const std::string& animation_name, int sprite_num, float interval) :
interval_(interval)
{
	for (int i = 1; i <= sprite_num; ++i) {
		sprites_.emplace_back(&(sprites->lookup(animation_name + "_" + std::to_string(i))));
	}
}
