#pragma once

#include <string>
#include <SDL.h>
#include <glm/fwd.hpp>
#include "../DrawSprites.hpp"
#include "../engine/Text.hpp"


class Dialog {
private:
	// each element represents the current player/NPC/Background who is speaking ("avator_null" is a special key
	// which means this is the narratage), used as keys to do the sprite lookup
	std::vector<std::string> avatar_sprites_;
	// Each element is a vector of string (split by '\n'), it represents the text of current speaking.
	// It is the same size of avatar_names
	std::vector<std::vector<std::string>> scripts_;
	// cur idx of scripts and avatar_sprites_
	int cur_script_idx_ = 0;
	// current showing text of scripts_[cur_script_idx_]: scripts_[cur_script_idx_][cur_sen_idx_, ..., cur_sen_idx_ + TEXT_LINES_PER_BOX)
	int cur_sen_idx_ = 0;

	bool start_phase_ = false;
	bool dialog_phase_ = false;
	bool exit_phase_ = false;

	// Used for animation
	float elapsed_since_last_char_ = 0.0f;
	int cur_animation_sen_idx_ = 0;
	int cur_char_idx_ = 0;

	Text text_;
	const bool auto_trigger_;

	std::string GenerateStr();
	void ResetForNextScript();
public:
	Dialog(bool auto_trigger);
	// Append a script to this dialog
	void Append(const std::string& text, const std::string& avatar_sprite);
	void Update(float elapsed);
	void Draw(const glm::uvec2& window_size);
	bool IsCompleted() const;
	void Reset();
	void RegisterKeyEvents();
	void UnregisterKeyEvents() const;
};