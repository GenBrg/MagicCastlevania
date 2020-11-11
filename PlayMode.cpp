#include "PlayMode.hpp"

#include "DrawLines.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include "Load.hpp"
#include "Sprite.hpp"
#include "DrawSprites.hpp"
#include "Util.hpp"
#include "engine/Timer.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <iostream>
#include <sstream>

PlayMode::PlayMode() :
cur_room(data_path("1.room")),
player(&cur_room),
hud(&player)
{
	player.SetPosition(glm::vec2(20.0f, 113.0f));
//	colliders.emplace_back(new Collider(glm::vec4(0.0f, 0.0f, 10000.0f, 60.0f), nullptr));
//	colliders.emplace_back(new Collider(glm::vec4(200.0f, 80.0f, 215.0f, 104.0f), nullptr));
	std::string test_str = "1abcdefghijklmnopqrstuvwxyz\"~!!!\n2abcdefghijklmnopqrstuvwxyz\"~!!!\n3abcdefghijklmnopqrstuvwxyz\"~!!!\n4abcdefghijklmnopqrstuvwxyz\"~!!!\n5abcdefghijklmnopqrstuvwxyz\"~!!!\n66666666666666\n7abcdefghijklmnopqrstuvwxyz\"~!!!\n888888888888888";
	dialog_p = new Dialog(test_str, "ghost_hurt_1", false);
}

PlayMode::~PlayMode() {
	delete dialog_p;
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	return InputSystem::Instance()->OnKeyEvent(evt);
}

void PlayMode::update(float elapsed) {
	InputSystem::Instance()->Update(elapsed);
	cur_room.Update(elapsed, &player);
	TimerManager::Instance().Update();

	dialog_p->Update(elapsed);
}

void PlayMode::draw(glm::uvec2 const &window_size) {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//use alpha blending:
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//don't use the depth test:
	glDisable(GL_DEPTH_TEST);

	{ //use a DrawSprites to do the drawing:
		DrawSprites draw(*sprites, VIEW_MIN, VIEW_MAX, window_size, DrawSprites::AlignPixelPerfect);
		Transform2D tranform_(nullptr);
		tranform_.position_ = glm::vec2(0.0f, 0.0f);
		draw.draw(*sprite_bg, tranform_);
		player.Draw(draw);
		cur_room.Draw(draw);
		hud.Draw(draw);
	}

	{ //Overlay some text:
		if(!dialog_p->ShouldExitDialog()) {
			dialog_p->Draw(window_size);
		}

//		float aspect = float(window_size.x) / float(window_size.y);
//		DrawLines lines(glm::mat4(
//			1.0f / aspect, 0.0f, 0.0f, 0.0f,
//			0.0f, 1.0f, 0.0f, 0.0f,
//			0.0f, 0.0f, 1.0f, 0.0f,
//			0.0f, 0.0f, 0.0f, 1.0f
//		));
//
//		auto draw_text = [&](glm::vec2 const &at, std::string const &text, float H) {
//			lines.draw_text(text,
//				glm::vec3(at.x, at.y, 0.0),
//				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
//				glm::u8vec4(0x00, 0x00, 0x00, 0x00));
//			float ofs = 2.0f / window_size.y;
//			lines.draw_text(text,
//				glm::vec3(at.x + ofs, at.y + ofs, 0.0),
//				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
//				glm::u8vec4(0xff, 0xff, 0xff, 0x00));
//		};
//
//		std::stringstream message;
//		message << "HP: " << player.GetHp();
//		draw_text(glm::vec2(-aspect + 0.5f, 0.9f), message.str(), 0.09f);

	}
	GL_ERRORS();
}
