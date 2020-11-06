// #pragma once

// #include <vector>
// #include <Sprite.hpp>
// #include "Load.hpp"

// inline std::unordered_map<std::string, std::vector<Sprite>> animationCollection;

// Load< void > load_animations(LoadTagDefault, []() {
// // init animationCollection here
// });

// class Animation {
// private:
// 	// points to a vector in the animation collection
// 	std::vector<Sprite>* animationP;
// 	// delay time of a single sprite
// 	float delay;
// 	// whether replay from start after playing the animation once
// 	bool loopPlay;
// 	int curSpriteIdx;
// 	float elapsedSinceStart;

// public:
// 	Sprite getCurSprite() const;

// 	void update(float elapsed);

// 	void start();
// 	void pause();

// 	// return true when loopPlay == false && curSpriteIdx == animationP->size() - 1
// 	bool complete();
// };

#pragma once

#include "../Sprite.hpp"
#include "../DrawSprites.hpp"
#include "Transform2D.hpp"

#include <vector>
#include <unordered_map>
#include <string>

class AnimationController {
public:
	using Animation = std::vector<const Sprite*>;

	AnimationController(Transform2D* parent_transform) :
	transform_(parent_transform)
	{}

	void PlayAnimation(const std::string& animation_name, float interval, bool loop);
	void Update(float elapsed);
	void Draw(DrawSprites& draw) const;

	static void LoadAnimation(const std::string& animation_name, int sprite_num);

private:
	static std::unordered_map<std::string, Animation> animation_collection;

	Transform2D transform_;
	Animation* animation_ { nullptr };
	const Sprite* current_sprite_ { nullptr };
	float interval_;
	float time_since_begin_ { 0.0f };
	float length_;

	bool playing_ { false };
	bool loop_ { false };
};
