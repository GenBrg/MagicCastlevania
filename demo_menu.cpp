#include "demo_menu.hpp"
#include "Load.hpp"
#include "Sprite.hpp"
#include "data_path.hpp"

#include "PlantMode.hpp"
#include "DemoLightingMultipassMode.hpp"
#include "DemoLightingForwardMode.hpp"
#include "DemoLightingDeferredMode.hpp"

Load< SpriteAtlas > trade_font_atlas(LoadTagDefault, []() -> SpriteAtlas const* {
	return new SpriteAtlas(data_path("trade-font"));
	});

std::shared_ptr< MenuMode > demo_menu;

Load< void > load_demo_menu(LoadTagDefault, []() {
	std::vector< MenuMode::Item > items;
	items.emplace_back("[[ DEMO MENU ]]");
	items.emplace_back("plant");
	items.back().on_select = [](MenuMode::Item const&) {
		Mode::set_current(std::make_shared< PlantMode >());
	};
	items.emplace_back("lighting - multipass");
	items.back().on_select = [](MenuMode::Item const&) {
		Mode::set_current(std::make_shared< DemoLightingMultipassMode >());
	};
	items.emplace_back("lighting - forward");
	items.back().on_select = [](MenuMode::Item const&) {
		Mode::set_current(std::make_shared< DemoLightingForwardMode >());
	};
	items.emplace_back("lighting - deferred");
	items.back().on_select = [](MenuMode::Item const&) {
		Mode::set_current(std::make_shared< DemoLightingDeferredMode >());
	};


	demo_menu = std::make_shared< MenuMode >(items);
	demo_menu->selected = 1;
	demo_menu->atlas = trade_font_atlas;
	demo_menu->view_min = glm::vec2(0.0f, 0.0f);
	demo_menu->view_max = glm::vec2(320.0f, 200.0f);

	demo_menu->layout_items(2.0f);

	demo_menu->left_select = &trade_font_atlas->lookup(">");
	demo_menu->left_select_offset = glm::vec2(-5.0f - 3.0f, 0.0f);
	demo_menu->right_select = &trade_font_atlas->lookup("<");
	demo_menu->right_select_offset = glm::vec2(0.0f + 3.0f, 0.0f);
	demo_menu->select_bounce_amount = 5.0f;
	});
