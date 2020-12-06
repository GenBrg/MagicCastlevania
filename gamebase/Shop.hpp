#pragma once

#include <gamebase/ItemPrototype.hpp>

#include <array>

class Shop {
public:
	inline static constexpr int kSlotNum = 12;
	void GenerateItems();
	void Purchase(size_t slot_num);
	ItemPrototype* PeekItem(size_t slot_num) const { return items_[slot_num]; }

private:
	std::array<ItemPrototype*, kSlotNum> items_ { nullptr };
};
