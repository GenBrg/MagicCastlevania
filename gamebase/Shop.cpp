#include "Shop.hpp"

#include <engine/Random.hpp>
#include <gamebase/Player.hpp>

#define ANIMATION_TRANSITION 0.1
#define MERCHANT_SPRITE_CNT 29


Shop::Shop(glm::vec2 pos): pos_(pos) {
    printf("pos.x=%f\n", pos_.x);
}

void Shop::GenerateItems()
{
	for (auto& item : items_) {
		int dice = static_cast<int>(Random::Instance()->Generate());
		if (dice < 0.5f) {
			int item_idx = static_cast<int>(ItemPrototype::GetPrototypeNum() * Random::Instance()->Generate());
			item = ItemPrototype::GetPrototype(item_idx);
		} else {
			item = nullptr;
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
		item = nullptr;
	}
}

void Shop::RegisterKeyEvents() {
    std::cout << "enter shop area, RegisterKeyEvents e" << std::endl;
    InputSystem::Instance()->Register(SDLK_e, [&](InputSystem::KeyState &key_state, float elapsed) {
        if (key_state.pressed) {
            key_state.pressed = false;
            std::cout << "Pressed enter shop" << std::endl;
            // change current mode to shop mode
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
