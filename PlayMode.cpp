#include "PlayMode.hpp"

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

PlayMode::PlayMode() {
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN || evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_LEFT) {
			//TODO...
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	//background face slowly rotates through [0,1):
	background_fade += elapsed / 10.0f;
	background_fade -= std::floor(background_fade);
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//--- set ppu state based on game state ---

	//background will be some hsv-like fade:
	ppu.background_color = glm::u8vec4(
		std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 0.0f / 3.0f) ) ) ))),
		std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 1.0f / 3.0f) ) ) ))),
		std::min(255,std::max(0,int32_t(255 * 0.5f * (0.5f + std::sin( 2.0f * M_PI * (background_fade + 2.0f / 3.0f) ) ) ))),
		0xff
	);

	//--- actually draw ---
	ppu.draw(drawable_size);
}
