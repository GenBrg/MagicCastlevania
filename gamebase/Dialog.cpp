#include "../engine/InputSystem.hpp"
#include "../Util.hpp"
#include "Dialog.hpp"
#include "TextBase.hpp"

#include <sstream>
#include <iostream>
#include <utility>
#include <algorithm>

Dialog::Dialog(const std::string &text, std::string  thumbnail_sprite, bool no_thumbnail):
no_thumbnail_(no_thumbnail),
thumbnail_sprite_(std::move(thumbnail_sprite))
{
	// split text
	std::stringstream ss(text);
	std::string single_text;
	while (std::getline(ss, single_text, '\n')) {
		texts.push_back(std::move(single_text));
	}
	texts.emplace_back("<Press X to exit>");
	tb = TextBase(data_path(FONT_FILE_NAME), LINE_HEIGHT);

	// handle down press
	InputSystem::Instance()->Register(SDLK_DOWN, [this](InputSystem::KeyState& key_state, float elapsed) {
		if (key_state.pressed) {
			key_state.pressed = false;
			if(cur_line_idx + TEXT_LINES_PER_BOX <= (int)texts.size()) {
				if(!pause_animation) {
					// when in middle of an animation, down is pressed, directly show the whole line
					cur_char_idx = (int)texts[cur_line_idx].size() - 1;
				} else {
					// proceed to the next line
					cur_line_idx++;
					if(cur_animation_line_idx < cur_line_idx + TEXT_LINES_PER_BOX) {
						// resume animation only when this line is not animated before
						pause_animation = false;
					}
				}
			}

			if(cur_animation_line_idx == (int)texts.size()) {
				//exit the current dialog
				complete_flag_ = true;
			}

		} else if (key_state.released) {
			key_state.released = false;
		}
	});

	// handle up press
	InputSystem::Instance()->Register(SDLK_UP, [this](InputSystem::KeyState& key_state, float elapsed) {
		if (key_state.pressed) {
			key_state.pressed = false;
			cur_line_idx = std::max(cur_line_idx-1, 0);
		} else if (key_state.released) {
			key_state.released = false;
		}
	});

	// handle X press
	InputSystem::Instance()->Register(SDLK_x, [this](InputSystem::KeyState& key_state, float elapsed) {
		if (key_state.pressed) {
			key_state.pressed = false;
			if(complete_flag_) {
				exit_flag_ = true;
			}
		} else if (key_state.released) {
			key_state.released = false;
		}
	});
}

//bool Dialog::OnKeyEvent(const SDL_Event &evt) {
//
//	return false;
//}

void Dialog::Update(float elapsed) {
	if(pause_animation) {
		return;
	}
	elapsed_since_last_char += elapsed;
	if(elapsed_since_last_char > ANIMATION_DELAY) {
		// need to update one char
		elapsed_since_last_char = 0.0f;

		cur_char_idx ++;
		if(cur_char_idx + 1 >= (int)texts[cur_animation_line_idx].size()) {
			// start animation of the next line
			cur_char_idx = 0;
			cur_animation_line_idx++;
		}
		if(cur_animation_line_idx >= cur_line_idx + TEXT_LINES_PER_BOX) {
			// means the animation of current screen completes, wait for user to press DOWN
			pause_animation = true;
		}
	}
}

void Dialog::Draw(const glm::uvec2 &window_size) {
	auto* drawSprites = new DrawSprites(*sprites, VIEW_MIN, VIEW_MAX, window_size, DrawSprites::AlignPixelPerfect);

	Transform2D transform_box(nullptr);
	transform_box.position_ = glm::vec2(DIALOG_BOX_LEFT, DIALOG_BOX_BOTTOM);
	transform_box.scale_ = glm::vec2(1.0f, 0.5f);

	Transform2D transform_thumbnail(nullptr);
	transform_thumbnail.position_ = glm::vec2(DIALOG_BOX_LEFT - DIALOG_BOX_TOP, DIALOG_BOX_BOTTOM);

	// draw a dialog background
	drawSprites->draw(sprites->lookup("dialog_box"), transform_box);
	if(!no_thumbnail_) {
		// draw a thumbnail img
		drawSprites->draw(sprites->lookup(thumbnail_sprite_), transform_thumbnail);
	}

	// force it draw
	delete drawSprites;

	tb.SetText(GenerateStr(),
	           FONT_SIZE,
			glm::u8vec4(0x00, 0x00, 0x00, 0xff),
			glm::vec2(DIALOG_BOX_LEFT + 20, DIALOG_BOX_TOP - 10));// the top left corner of the tex


	tb.Draw(window_size);
}

std::string Dialog::GenerateStr() {
	std::stringstream ss;
//	std::cout<<"cur_line_idx="<<cur_line_idx<<" cur_animation_line_idx="<<cur_animation_line_idx
//	<<" pause_animation="<<pause_animation<<std::endl;

	for(int i=cur_line_idx; i <= cur_animation_line_idx &&
							i < (int)texts.size() &&
							i < cur_line_idx + TEXT_LINES_PER_BOX
							; i++) {
		if(i == cur_animation_line_idx) {
			ss<<texts[i].substr(0, cur_char_idx + 1);
		} else {
			if(i == cur_line_idx + TEXT_LINES_PER_BOX - 1) {
				ss<<texts[i];
			} else {
				ss<<texts[i]<<"\n";
			}
		}
	}
//	std::cout<<"======Begin====\n"<<ss.str()<<"\n========eNd======\n";
	return ss.str();
}

bool Dialog::ShouldExitDialog() {
	return exit_flag_;
}