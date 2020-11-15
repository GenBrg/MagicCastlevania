#pragma once

#include "../Sprite.hpp"
#include "../DrawSprites.hpp"
#include "Transform2D.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>

class Animation {
	friend class AnimationController;
public:
	float GetLength() const { return interval_ * sprites_.size(); }
	static void LoadAnimation(const std::string& config_file_name);
	static Animation* GetAnimation(const std::string& name) { return &animation_collection.at(name); }

private:
	inline static std::unordered_map<std::string, Animation> animation_collection;

	Animation(const std::string& animation_name, int sprite_num, float interval);

	std::vector<const Sprite*> sprites_;
	float interval_;
};

class AnimationController {
public: 
	AnimationController(Transform2D* parent_transform) :
	transform_(parent_transform)
	{}

	void PlayAnimation(Animation* animation, bool loop, bool force_replay = true);
	void Update(float elapsed);
	void Draw(DrawSprites& draw) const;
	float GetLength() const { return length_; }
	
private:
	Transform2D transform_;
	Animation* animation_ { nullptr };
	const Sprite* current_sprite_ { nullptr };
	float time_since_begin_ { 0.0f };
	float length_;
	bool loop_;

	bool playing_ { false };
};
