#include "Shop.hpp"

#include <engine/Random.hpp>
#include <gamebase/Player.hpp>

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