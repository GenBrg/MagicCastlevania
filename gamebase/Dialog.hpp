#pragma once

#include <string>
#include <SDL.h>
#include <glm/fwd.hpp>
#include "../DrawSprites.hpp"
#include "TextBase.hpp"

#define DIALOG_BOX_LEFT (INIT_WINDOW_W * 0.25f)
#define DIALOG_BOX_RIGHT (INIT_WINDOW_W * 0.75f)
#define DIALOG_BOX_TOP (INIT_WINDOW_H * 0.25f)
#define DIALOG_BOX_BOTTOM 2
#define TEXT_LINES_PER_BOX 3

class Dialog {
private:
	// splitted strings based on '\n'
	std::vector<std::string> texts;
	// current showing text: texts[cur_idx, ..., cur_idx + TEXT_LINES_PER_BOX)
	int cur_idx;
	InputSystem input_system_;
	TextBase tb;

	std::string GenerateStr();
public:
	explicit Dialog(const std::string& text);
	Dialog();
	/**
	 * Every time the arrow down key is pressed, it will proceed to read the following text
	 * @param evt
	 * @return
	 */
	bool OnKeyEvent(const SDL_Event& evt);
	void Update(float elapsed);
	void Draw(const glm::uvec2& window_size);
	bool DialogComplete();
};