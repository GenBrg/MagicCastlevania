#include "../engine/InputSystem.hpp"
#include "../Util.hpp"
#include "Dialog.hpp"

#include <sstream>
#include <utility>
#include <algorithm>

#define AVATAR_PLAYER_SPRITE "avatar_player"
#define DIALOG_BOX_SPRITE "dialog_box"
#define AVATAR_BOARDER "square_border_big_full"
#define AVATAR_NULL "avatar_null"

// the dialog box is hardcoded to be at the bottom middle in the window
#define DIALOG_BOX_WIDTH (0.5f * INIT_WINDOW_W)
#define DIALOG_BOX_HEIGHT (0.25f * INIT_WINDOW_H)

#define TEXT_LINES_PER_BOX 3
#define LINE_HEIGHT 38
#define FONT_SIZE 1500
#define FONT_FILE_NAME "DigitalDisco.ttf"

#define ANIMATION_DELAY 0.05f


// get the coordinate of the dialog box (bottom left corner in pixel)
glm::vec2 GetDialogBottomLeft() {
	float x = (INIT_WINDOW_W - DIALOG_BOX_WIDTH) * 0.5f;
	return glm::vec2(x, 0);
}

Dialog::Dialog(bool auto_trigger) :
		text_(data_path(FONT_FILE_NAME)),
		auto_trigger_(auto_trigger)
{
	text_.SetFontSize(FONT_SIZE)
			.SetColor(glm::u8vec4(0x00, 0x00, 0x00, 0xff))
			.SetPos(glm::vec2(GetDialogBottomLeft().x + 20, DIALOG_BOX_HEIGHT - LINE_HEIGHT));
	Reset();
}

void Dialog::Update(float elapsed) {
	if(exit_phase_ || !start_phase_) {
		return;
	}

	if (cur_animation_sen_idx_ == (int) scripts_[cur_script_idx_].size()) {
		// complete animation
		return;
	}
	elapsed_since_last_char_ += elapsed;
	if (elapsed_since_last_char_ > ANIMATION_DELAY) {
		// need to update one char
		elapsed_since_last_char_ = 0.0f;

		cur_char_idx_++;
		if (cur_char_idx_ + 1 >= (int) scripts_[cur_script_idx_][cur_animation_sen_idx_].size()) {
			// start animation of the next line
			cur_char_idx_ = 0;
			cur_animation_sen_idx_++;
		}
	}
}

void Dialog::Draw(const glm::uvec2 &window_size) {
	if(exit_phase_ || !start_phase_) {
		return;
	}

	DrawSprites *draw_sprite = new DrawSprites(*sprites, VIEW_MIN, VIEW_MAX, window_size,
	                                           DrawSprites::AlignPixelPerfect);
	Transform2D transform_box(nullptr);
	transform_box.position_ = GetDialogBottomLeft();
	auto sprite = sprites->lookup(DIALOG_BOX_SPRITE);
	transform_box.scale_ = glm::vec2(DIALOG_BOX_WIDTH / sprite.size_px.x, DIALOG_BOX_HEIGHT / sprite.size_px.y);
	// draw a dialog background
	draw_sprite->draw(sprite, transform_box);

	if (avatar_sprites_[cur_script_idx_] != AVATAR_NULL) {
		// this needs a avatar
		Transform2D transform_avatar_boarder(nullptr), transform_avatar(nullptr);
		auto sprite_avatar = sprites->lookup(avatar_sprites_[cur_script_idx_]);
		auto sprite_avatar_boarder = sprites->lookup(AVATAR_BOARDER);
		float scale_para =
				DIALOG_BOX_HEIGHT / sprite_avatar.size_px.x * 0.9f; // 0.9 make it a little smaller than dialog box
		transform_avatar_boarder.scale_ = glm::vec2(scale_para, scale_para);
		transform_avatar.scale_ = transform_avatar_boarder.scale_;

		if (avatar_sprites_[cur_script_idx_] == AVATAR_PLAYER_SPRITE) {
			// place it left
			transform_avatar_boarder.position_ = glm::vec2(GetDialogBottomLeft().x - 0.5 * DIALOG_BOX_HEIGHT,
			                                               DIALOG_BOX_HEIGHT * 0.05f);
			transform_avatar.position_ = transform_avatar_boarder.position_;
		} else {
			// place it right
			transform_avatar_boarder.scale_.x = -transform_avatar_boarder.scale_.x;
			transform_avatar_boarder.position_ = glm::vec2(
					GetDialogBottomLeft().x + DIALOG_BOX_WIDTH + 0.5 * DIALOG_BOX_HEIGHT,
					DIALOG_BOX_HEIGHT * 0.05f);
			transform_avatar.position_ = transform_avatar_boarder.position_;
		}

		// draw a avatar img
		draw_sprite->draw(sprite_avatar, transform_avatar);
		draw_sprite->draw(sprite_avatar_boarder, transform_avatar_boarder);
	}
	// force it draw
	delete draw_sprite;
	// force it draw
	text_.SetText(GenerateStr());

	text_.Draw();
}

std::string Dialog::GenerateStr() {
	std::stringstream ss;

	for (int i = cur_sen_idx_; i <= cur_animation_sen_idx_ &&
	                           i < (int) scripts_[cur_script_idx_].size() &&
	                           i < cur_sen_idx_ + TEXT_LINES_PER_BOX; i++) {
		if (i == cur_animation_sen_idx_) {
			ss << scripts_[cur_script_idx_][i].substr(0, cur_char_idx_ + 1);
		} else {
			if (i == cur_sen_idx_ + TEXT_LINES_PER_BOX - 1) {
				ss << scripts_[cur_script_idx_][i];
			} else {
				ss << scripts_[cur_script_idx_][i] << "\n";
			}
		}
	}
	return ss.str();
}

bool Dialog::IsCompleted() const {
	return exit_phase_;
}

void Dialog::Append(const std::string &text, const std::string &avatar_sprite) {
	avatar_sprites_.push_back(avatar_sprite);

	// split text
	std::stringstream ss(text);
	std::string single_text;
	std::vector<std::string> script;
	while (std::getline(ss, single_text, '\n')) {
		script.push_back(std::move(single_text));
	}
	script.emplace_back("<Press e to continue>");

	scripts_.push_back(script);
}

void Dialog::Reset() {
	cur_script_idx_ = 0;
	cur_sen_idx_ = 0;
	if(auto_trigger_) {
		start_phase_ = true;
	} else {
		start_phase_ = false;
	}
	dialog_phase_ = false;
	exit_phase_ = false;
	cur_animation_sen_idx_ = 0;
	cur_char_idx_ = 0;
	elapsed_since_last_char_ = 0.0f;
}

void Dialog::ResetForNextScript() {
	assert(cur_script_idx_ + 1 < (int) scripts_.size());
	cur_script_idx_++;
	cur_sen_idx_ = 0;
	cur_animation_sen_idx_ = 0;
	cur_char_idx_ = 0;
	elapsed_since_last_char_ = 0.0f;
}

void Dialog::RegisterKeyEvents() {
	/* Register key events */
	// handle down press
	InputSystem::Instance()->Register(SDLK_DOWN, [this](InputSystem::KeyState &key_state, float elapsed) {
		if (key_state.pressed) {
            Sound::play(*sound_samples["select"]);
            key_state.pressed = false;
			if (cur_animation_sen_idx_ != (int) scripts_[cur_script_idx_].size()) {
				// when in middle of an animation, down is pressed, directly show the whole script
				cur_animation_sen_idx_ = (int) scripts_[cur_script_idx_].size();
			} else {
				// proceed to the next line
				cur_sen_idx_ = std::min(cur_sen_idx_ + 1, (int) scripts_[cur_script_idx_].size() - 1);
			}
		} else if (key_state.released) {
			key_state.released = false;
		}
	});

	// handle up press
	InputSystem::Instance()->Register(SDLK_UP, [this](InputSystem::KeyState &key_state, float elapsed) {
		if (key_state.pressed) {
            Sound::play(*sound_samples["select"]);
            key_state.pressed = false;
			cur_sen_idx_ = std::max(cur_sen_idx_ - 1, 0);
		} else if (key_state.released) {
			key_state.released = false;
		}
	});

	// handle e press
	InputSystem::Instance()->Register(SDLK_e, [this](InputSystem::KeyState &key_state, float elapsed) {
		if (key_state.pressed) {
            Sound::play(*sound_samples["select"]);
            key_state.pressed = false;

			if (start_phase_ && !dialog_phase_) {
				// from start phase to dialog phase
				dialog_phase_ = true;
			}

			if (dialog_phase_ && cur_animation_sen_idx_ != (int) scripts_[cur_script_idx_].size()) {
				// when in middle of an animation, down is pressed, directly show the whole script
				cur_animation_sen_idx_ = (int) scripts_[cur_script_idx_].size();
			} else {
				// proceed to the next 3 lines
				cur_sen_idx_ = std::min(cur_sen_idx_ + 3, (int) scripts_[cur_script_idx_].size() - 1);
			}

			if (cur_sen_idx_ + 1 >= (int) scripts_[cur_script_idx_].size()) {
				// complete one script
				if (cur_script_idx_ == (int) scripts_.size() - 1) {
					// if there are not other scripts left
					exit_phase_ = true;
				} else {
					ResetForNextScript();
				}
			}
			// make sure already start
			start_phase_ = true;
		} else if (key_state.released) {
			key_state.released = false;
		}
	});
}

void Dialog::UnregisterKeyEvents() const {
	InputSystem::Instance()->Unregister(SDLK_x);
	InputSystem::Instance()->Unregister(SDLK_UP);
	InputSystem::Instance()->Unregister(SDLK_DOWN);
}
