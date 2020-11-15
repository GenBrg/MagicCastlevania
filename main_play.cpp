#include "main_play.hpp"
#include "Load.hpp"

std::shared_ptr< PlayMode > main_play;

Load< void > load_main_play(LoadTagLate, []() {
	main_play = std::make_shared< PlayMode >();
	});
