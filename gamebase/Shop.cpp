#include "Shop.hpp"
#include "../MenuMode.hpp"
#include <engine/Random.hpp>
#include <gamebase/Player.hpp>

#define ANIMATION_TRANSITION 0.1
#define MERCHANT_SPRITE_CNT 29


Shop::Shop(glm::vec2 pos): pos_(pos) {
    printf("pos.x=%f\n", pos_.x);
}

void Shop::GenerateItems(size_t level)
{
	items_[0] = ItemPrototype::GetPrototype("sword" + std::to_string(level));
	items_[1] = ItemPrototype::GetPrototype("shield" + std::to_string(level));
	items_[2] = ItemPrototype::GetPrototype("vest" + std::to_string(level));
	items_[3] = ItemPrototype::GetPrototype("boot" + std::to_string(level));

	for (size_t i = 4; i < kSlotNum; ++i) {
		int dice = static_cast<int>(Random::Instance()->Generate());
		if (dice < 0.5f) {
			bool is_potion = false;
			while (!is_potion) {
				int item_idx = static_cast<int>(ItemPrototype::GetPrototypeNum() * Random::Instance()->Generate());
				items_[i] = ItemPrototype::GetPrototype(item_idx);
				is_potion = dynamic_cast<PotionPrototype*>(items_[i]);
			}
		} else {
			items_[i] = nullptr;
		}
	}
}

void Shop::Purchase(size_t slot_num)
{
	ItemPrototype* item = items_[slot_num];
	if (item) {
		// Backpack full
		if (player->IsInventoryFull()) {
			return;
		}

		int price = item->GetPrice();

		// Insufficient money
		if (player->GetCoin() < price) {
			return;
		}

		// Succeed
		player->AddCoin(-price);
		assert(player->PickupItem(item));
		items_[slot_num] = nullptr;
	}
}

void Shop::RegisterKeyEvents() {
    std::cout << "enter shop area, RegisterKeyEvents e" << std::endl;
    InputSystem::Instance()->Register(SDLK_e, [&](InputSystem::KeyState &key_state, float elapsed) {
        if (key_state.pressed) {
            key_state.pressed = false;
            std::cout << "Pressed enter shop" << std::endl;
			std::vector< MenuMode::Item > items;
			items.emplace_back("Static", &sprites->lookup("shop_window"), nullptr);
			for (size_t i = 0; i < 12; i++) {
				items.emplace_back("Item_" + std::to_string(i), nullptr, &sprites->lookup("select_target"));
			}
			std::shared_ptr< MenuMode > shop_menu;
			shop_menu = std::make_shared< MenuMode >(items, 3);
			shop_menu->selected = 1;
			shop_menu->atlas = sprites;
			shop_menu->view_min = glm::vec2(0.0f, 0.0f);
			shop_menu->view_max = glm::vec2(960.0f, 541.0f);
			shop_menu->grid_layout_items(glm::vec2(582.0f, 394.0f), 77.0f, 80.0f, 1, 13);
			Mode::set_current(shop_menu);
        }
    });
}

void Shop::UnregisterKeyEvents() {
    std::cout << "leave shop area, UnregisterKeyEvents e" << std::endl;
    InputSystem::Instance()->Unregister(SDLK_e);
}

void Shop::Draw(DrawSprites& draw) {
    int idx = int(elapsed_ / ANIMATION_TRANSITION) % MERCHANT_SPRITE_CNT + 1;
    auto transform = Transform2D(nullptr);
    transform.position_ = pos_;
    draw.draw(sprites->lookup("merchant_idle_" + std::to_string(idx)), transform);
    transform.position_.x += 50;
    draw.draw(sprites->lookup("merchant_chest"), transform);
}

void Shop::Update(float elapsed) {
    elapsed_ += elapsed;
}
