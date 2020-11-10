#pragma once

#include <string>
#include <SDL.h>
#include <glm/fwd.hpp>
#include "../DrawSprites.hpp"
#include "../engine/Text.hpp"

#define DIALOG_BOX_LEFT (INIT_WINDOW_W * 0.25f)
#define DIALOG_BOX_RIGHT (INIT_WINDOW_W * 0.75f)
#define DIALOG_BOX_TOP (INIT_WINDOW_H * 0.25f)
#define DIALOG_BOX_BOTTOM 0
#define TEXT_LINES_PER_BOX 4
#define LINE_HEIGHT 38
#define FONT_SIZE 2300

#define FONT_FILE_NAME "ReallyFree-ALwl7.ttf"

#define ANIMATION_DELAY 0.05f

class Dialog {
private:
	// splitted strings based on '\n'
	std::vector<std::string> texts;
	// if true, this is the background store from no body, thumbnail_sprite_ will be ignored
	const bool no_thumbnail_;
	// which charactor is speaking this dialog
	const std::string thumbnail_sprite_;
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
	Dialog(const std::string& text, std::string thumbnail_sprite, bool no_thumbnail);
	/**
	 * Every time the arrow down key is pressed, it will proceed to read the following text
	 * @param evt
	 * @return
	 */
	void Update(float elapsed);
	void Draw(const glm::uvec2& window_size);
	bool ShouldExitDialog();
};