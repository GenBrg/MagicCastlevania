#include "Mode.hpp"
#include "gamebase/Player.hpp"
#include "gamebase/HeadsUpDisplay.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>
#include "gamebase/Room.hpp"
// #include <gamebase/Room.hpp>
// #include <gamebase/MainPlayer.hpp>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, space;

	Room cur_room;
	Player player;
	HeadsUpDisplay hud;
};
