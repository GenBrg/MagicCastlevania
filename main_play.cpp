#include "main_play.hpp"
#include "Load.hpp"

std::shared_ptr< PlayMode > main_play;

Load< void > load_main_play(LoadTagDefault, []() {
	main_play = std::make_shared< PlayMode >();
	});
