#include "main_menu.hpp"
#include "Load.hpp"
#include "Sprite.hpp"
#include "data_path.hpp"
#include "PlayMode.hpp"
#include "main_play.hpp"
std::shared_ptr< MenuMode > main_menu;

Load< void > load_main_menu(LoadTagLate, []() {
	std::vector< MenuMode::Item > items;
	items.emplace_back("[[ DEMO MENU ]]" ,&sprites->lookup("text_button_1"),&sprites->lookup("text_button_1_selected"));

	items.emplace_back("Play", &sprites->lookup("text_button_1"), &sprites->lookup("text_button_1_selected"));
	items.back().on_select = [](MenuMode::Item const&) {
		Mode::set_current(main_play);
	};

	items.emplace_back("Play 2", &sprites->lookup("text_button_1"), &sprites->lookup("text_button_1_selected"));
	items.back().on_select = [](MenuMode::Item const&) {
		Mode::set_current(main_play);
	};

	items.emplace_back("Play 3", &sprites->lookup("text_button_1"), &sprites->lookup("text_button_1_selected"));
	items.back().on_select = [](MenuMode::Item const&) {
		Mode::set_current(main_play);
	};

	main_menu = std::make_shared< MenuMode >(items);
	main_menu->selected = 1;
	main_menu->atlas = sprites;
	main_menu->view_min = glm::vec2(0.0f, 0.0f);
	main_menu->view_max = glm::vec2(960.0f, 541.0f);

	main_menu->vertical_layout_items(2.0f);
	});
