#include "../engine/InputSystem.hpp"
#include "../Util.hpp"
#include "Dialog.hpp"

#include <sstream>
#include <utility>
#include <algorithm>

#define AVATAR_PLAYER_SPRITE "avatar_player"
#define DIALOG_BOX_SPRITE "dialog_box"
#define AVATAR_BOARDER "square_border_big_full"

// the dialog box is hardcoded to be at the bottom middle in the window
#define DIALOG_BOX_WIDTH (0.5f * INIT_WINDOW_W)
#define DIALOG_BOX_HEIGHT (0.25f * INIT_WINDOW_H)

#define TEXT_LINES_PER_BOX 3
#define LINE_HEIGHT 38
#define FONT_SIZE 2300
#define FONT_FILE_NAME "ReallyFree-ALwl7.ttf"

#define ANIMATION_DELAY 0.05f


// get the coordinate of the dialog box (bottom left corner in pixel)
glm::vec2 GetDialogBottomLeft() {
	float x = (INIT_WINDOW_W - DIALOG_BOX_WIDTH) * 0.5f;
	return glm::vec2(x, 0);
}

Dialog::Dialog(const std::string &text, std::string  avatar_sprite, bool no_avatar):
no_avatar_(no_avatar),
avatar_sprite_(std::move(avatar_sprite)),
text_(data_path(FONT_FILE_NAME), nullptr)
{
	// split text
	std::stringstream ss(text);
	std::string single_text;
	while (std::getline(ss, single_text, '\n')) {
		texts.push_back(std::move(single_text));
	}
	texts.emplace_back("<Press X to exit>");

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

	text_.SetFontSize(FONT_SIZE)
	     .SetColor(glm::u8vec4(0x00, 0x00, 0x00, 0xff))
	     .SetPos(glm::vec2(GetDialogBottomLeft().x + 20,  DIALOG_BOX_HEIGHT - LINE_HEIGHT));
}

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
	transform_box.position_ = GetDialogBottomLeft();
	auto sprite = sprites->lookup(DIALOG_BOX_SPRITE);
	transform_box.scale_ = glm::vec2(DIALOG_BOX_WIDTH / sprite.size_px.x, DIALOG_BOX_HEIGHT / sprite.size_px.y);
	// draw a dialog background
	drawSprites->draw(sprite, transform_box);

	if(!no_avatar_) {
		Transform2D transform_avatar_boarder(nullptr), transform_avatar(nullptr);
		auto sprite_avatar = sprites->lookup(avatar_sprite_);
		auto sprite_avatar_boarder = sprites->lookup(AVATAR_BOARDER);
		float scale_para = DIALOG_BOX_HEIGHT / sprite_avatar.size_px.x * 0.9f; // 0.9 make it a little smaller than dialog box
		transform_avatar_boarder.scale_ = glm::vec2(scale_para,scale_para);
		transform_avatar.scale_ = transform_avatar_boarder.scale_;

		if(avatar_sprite_ == AVATAR_PLAYER_SPRITE) {
			// place it left
			transform_avatar_boarder.position_ = glm::vec2(GetDialogBottomLeft().x - 0.5 * DIALOG_BOX_HEIGHT,
												  DIALOG_BOX_HEIGHT * 0.05f);
			transform_avatar.position_ = transform_avatar_boarder.position_;
		} else {
			// place it right
			transform_avatar_boarder.scale_.x = - transform_avatar_boarder.scale_.x;
			transform_avatar_boarder.position_ = glm::vec2(GetDialogBottomLeft().x + DIALOG_BOX_WIDTH + 0.5 * DIALOG_BOX_HEIGHT,
												  DIALOG_BOX_HEIGHT * 0.05f);
			transform_avatar.position_ = transform_avatar_boarder.position_;
		}

		// draw a avatar img
		drawSprites->draw(sprite_avatar, transform_avatar);
		drawSprites->draw(sprite_avatar_boarder, transform_avatar_boarder);
	}

	// force it draw
	delete drawSprites;

	text_.SetText(GenerateStr());
	text_.Draw(window_size);
}

std::string Dialog::GenerateStr() {
	std::stringstream ss;

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
	return ss.str();
}

bool Dialog::ShouldExitDialog() const {
	return exit_flag_;
}
