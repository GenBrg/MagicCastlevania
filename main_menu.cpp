#include "main_menu.hpp"
#include "Load.hpp"
#include "Sprite.hpp"
#include "data_path.hpp"
#include "PlayMode.hpp"
#include "main_play.hpp"
std::shared_ptr< MenuMode > main_menu;

Load< void > load_main_menu(LoadTagLate, []() {
	std::vector< MenuMode::Item > items;
	items.emplace_back("Static" ,&sprites->lookup("menu_static"),&sprites->lookup("menu_static"));
	items.emplace_back("Start", &sprites->lookup("start_button"), &sprites->lookup("start_button_selected"));
	items.back().on_select = [](MenuMode::Item const&) {
		Mode::set_current(main_play);
	};
	items.back().transform.position_ = glm::vec2(396.0f + 83.0f, 541.0f - (358.0f + 18.0f));

	items.emplace_back("Quit", &sprites->lookup("quit_button"), &sprites->lookup("quit_button_selected"));
	items.back().on_select = [](MenuMode::Item const&) {
		exit(0);
	};
	items.back().transform.position_ = glm::vec2(396.0f + 83.0f, 541.0f - (417.0f + 18.0f));

	main_menu = std::make_shared< MenuMode >(items);
	main_menu->selected = 1;
	main_menu->atlas = sprites;
	main_menu->view_min = glm::vec2(0.0f, 0.0f);
	main_menu->view_max = glm::vec2(960.0f, 541.0f);

	});
