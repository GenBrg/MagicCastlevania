#include <main_play.hpp>
#include <Load.hpp>
#include <gamebase/Door.hpp>
#include <engine/Animation.hpp>

std::shared_ptr< PlayMode > main_play;

Load< void > load_main_play(LoadTagLate, []() {
	Door::opened_animation_ = Animation::GetAnimation("door_opened");
	Door::opening_animation_ = Animation::GetAnimation("door");
	Door::closed_animation_ = Animation::GetAnimation("door_closed");
	main_play = std::make_shared< PlayMode >();
});
