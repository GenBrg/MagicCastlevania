#include "PlayMode.hpp"

#include "DrawLines.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include "Load.hpp"
#include "Sprite.hpp"
#include "DrawSprites.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

Sprite const *sprite_dunes = nullptr;

Load< SpriteAtlas > sprites(LoadTagDefault, []() -> SpriteAtlas const * {
	SpriteAtlas const *ret = new SpriteAtlas(data_path("MagicCastlevania"));

	sprite_dunes = &ret->lookup("dunes-ship");

	return ret;
});

PlayMode::PlayMode() {
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.repeat) {
			//ignore repeats
		} else if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//use alpha blending:
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//don't use the depth test:
	glDisable(GL_DEPTH_TEST);

	{ //use a DrawSprites to do the drawing:
		DrawSprites draw(*sprites, view_min, view_max, drawable_size, DrawSprites::AlignPixelPerfect);
		glm::vec2 ul = glm::vec2(view_max.x / 2.0f, view_max.y / 2.0f);
		draw.draw(*sprite_dunes, ul, glm::radians(0.0f));
		draw.draw(*sprite_dunes, ul, glm::radians(20.0f));
		draw.draw(*sprite_dunes, ul, glm::radians(40.0f));
		draw.draw(*sprite_dunes, ul, glm::radians(60.0f));
	}

	{ //use DrawLines to overlay some text:
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		// auto draw_text = [&](glm::vec2 const &at, std::string const &text, float H) {
		// 	lines.draw_text(text,
		// 		glm::vec3(at.x, at.y, 0.0),
		// 		glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
		// 		glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		// 	float ofs = 2.0f / drawable_size.y;
		// 	lines.draw_text(text,
		// 		glm::vec3(at.x + ofs, at.y + ofs, 0.0),
		// 		glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
		// 		glm::u8vec4(0xff, 0xff, 0xff, 0x00));
		// };

		// draw_text(glm::vec2(-aspect + 0.1f, 0.0f), server_message, 0.09f);

		// draw_text(glm::vec2(-aspect + 0.1f,-0.9f), "(press WASD to change your total)", 0.09f);
	}
	GL_ERRORS();
}
