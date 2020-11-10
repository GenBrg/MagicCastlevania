#pragma once

#include <string>
#include <SDL.h>
#include <glm/fwd.hpp>
#include "../DrawSprites.hpp"
#include "../engine/Text.hpp"


class Dialog {
private:
	// splitted strings based on '\n'
	std::vector<std::string> texts;
	// if true, this is the background store from no body, avatar_sprite_ will be ignored
	const bool no_avatar_;
	// which charactor is speaking this dialog
	const std::string avatar_sprite_;
	// current showing text: texts[cur_line_idx, ..., cur_line_idx + TEXT_LINES_PER_BOX)
	int cur_line_idx = 0;
	Text text_;

	bool complete_flag_ = false;
	bool exit_flag_ = false;

	// Used for animation
	int cur_animation_line_idx = 0;
	int cur_char_idx = 0;
	float elapsed_since_last_char = 0.0f;
	bool pause_animation = false;


	std::string GenerateStr();
public:
	Dialog(const std::string& text, std::string avatar_sprite, bool no_avatar);
	/**
	 * Every time the arrow down key is pressed, it will proceed to read the following text
	 * @param evt
	 * @return
	 */
	void Update(float elapsed);
	void Draw(const glm::uvec2& window_size);
	bool ShouldExitDialog() const;
};