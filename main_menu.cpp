#include "main_menu.hpp"
#include "Load.hpp"
#include "Sprite.hpp"
#include "data_path.hpp"

#include "PlayMode.hpp"

std::shared_ptr< MenuMode > main_menu;

Load< void > load_main_menu(LoadTagDefault, []() {
	std::vector< MenuMode::Item > items;
	items.emplace_back("[[ DEMO MENU ]]" ,&sprites->lookup("text_button_1"),&sprites->lookup("text_button_selected"));
	/*items.back().sprite = sprites->lookup("text_button_1");
	items.back().sprite_selected = sprites->lookup("text_button_selected");

	items.emplace_back("Play 1");
	items.back().on_select = [](MenuMode::Item const&) {
		Mode::set_current(std::make_shared< PlayMode >());
	};
	items.back().sprite = sprites->lookup("text_button_1");
	items.back().sprite_selected = sprites->lookup("text_button_selected");
	items.emplace_back("Play 2");
	items.back().on_select = [](MenuMode::Item const&) {
		Mode::set_current(std::make_shared< PlayMode >());
	};
	items.back().sprite = sprites->lookup("text_button_1");
	items.back().sprite_selected = sprites->lookup("text_button_selected");
	items.emplace_back("Play 3");
	items.back().on_select = [](MenuMode::Item const&) {
		Mode::set_current(std::make_shared< PlayMode >());
	};
	items.back().sprite = sprites->lookup("text_button_1");
	items.back().sprite_selected = sprites->lookup("text_button_selected");
	items.emplace_back("Play 4");
	items.back().on_select = [](MenuMode::Item const&) {
		Mode::set_current(std::make_shared< PlayMode >());
	};
	items.back().sprite = sprites->lookup("text_button_1");
	items.back().sprite_selected = sprites->lookup("text_button_selected");*/


	main_menu = std::make_shared< MenuMode >(items);
	main_menu->selected = 1;
	main_menu->atlas = sprites;
	main_menu->view_min = glm::vec2(0.0f, 0.0f);
	main_menu->view_max = glm::vec2(960.0f, 541.0f);

	main_menu->vertical_layout_items(2.0f);
	});
