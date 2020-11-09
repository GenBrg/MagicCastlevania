#include <sstream>
#include <iostream>
#include "../engine/InputSystem.hpp"
#include "../Util.hpp"
#include "Dialog.hpp"
#include "TextBase.hpp"

Dialog::Dialog(const std::string &text):cur_idx(0) {
	// split text
	std::stringstream ss(text);
	std::string single_text;
	while (std::getline(ss, single_text, '\n')) {
		texts.push_back(std::move(single_text));
	}
	cur_idx++;
	tb = TextBase(data_path("ReallyFree-ALwl7.ttf"), 40);

	//
//	input_system_.Register(SDLK_DOWN, [this](InputSystem::KeyState& key_state, float elapsed) {
//		if (key_state.pressed) {
//			key_state.pressed = false;
//		} else if (key_state.released) {
//			movement_component_.ReleaseJump();
//			key_state.released = false;
//		}
//	});
}

Dialog::Dialog() {

}

bool Dialog::OnKeyEvent(const SDL_Event &evt) {

	return false;
}

void Dialog::Update(float elapsed) {
//	if()

}

void Dialog::Draw(const glm::uvec2 &window_size) {
	auto* draw = new DrawSprites(*sprites, VIEW_MIN, VIEW_MAX, window_size, DrawSprites::AlignPixelPerfect);

	Transform2D transform(nullptr);
	transform.position_ = glm::vec2(DIALOG_BOX_LEFT, DIALOG_BOX_BOTTOM);
	transform.scale_ = glm::vec2(5.0f, 6.0f);

	// draw a dialog background
	draw->draw(sprites->lookup("hp_bar"), transform);
	// make it draw
	delete draw;

	tb.SetText(GenerateStr(),
//			"draw->draw(sprites->lookup(\"hp_bar\"), transform);\nadraw->draw(sprites->lookup(\"hp_bar\"), transform);\ndraw->draw(sprites->lookup(\"hp_bar\"), transform);\nutil::PrintVec2(transformed_anchor);",
			2300,
			glm::u8vec4(0xff, 0xff, 0xff, 0xff),
			glm::vec2(DIALOG_BOX_LEFT + 5, DIALOG_BOX_TOP - 5));// the top left corner of the text
	tb.Draw(window_size);
}

std::string Dialog::GenerateStr() {
	std::stringstream ss;
	for(int i=cur_idx; i<cur_idx + TEXT_LINES_PER_BOX && i < (int)texts.size(); i++) {
		ss<<texts[i]<<"\n";
	}
	return ss.str();
}

bool Dialog::DialogComplete() {
	return cur_idx + TEXT_LINES_PER_BOX >= (int)texts.size();
}