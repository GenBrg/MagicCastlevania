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

#include "Sprite.hpp"
#include "DrawSprites.hpp"

#include <vector>
#include <unordered_map>

using Animation = std::vector<Sprite*>;

extern std::unordered_map<std::string, Animation> animation_collection;

class AnimationController {
public:
	AnimationController(const std::string& animation_name);

	void Update(float elapsed);
	void Draw(DrawSprites& draw) const;

private:
	Animation* animation;
	size_t current_sprite;
};