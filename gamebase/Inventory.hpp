#pragma once

#include <gamebase/EquipmentPrototype.hpp>
#include <gamebase/PotionPrototype.hpp>

#include <vector>

class Inventory {
public:
	inline static constexpr size_t kEquipmentSlotNum { 4 };
	inline static constexpr size_t kBackpackSlotNum { 12 };

	bool PushItem(ItemPrototype* item);

	ItemPrototype* PeekItem(size_t slot_num);
	EquipmentPrototype* PeekEquipment(size_t slot_num);
	ItemPrototype* PopItem(size_t slot_num);
	EquipmentPrototype* PopEquipment(size_t slot_num);

	void UseItem(Player* player, size_t slot_num);
	void UnequipItem(size_t slot_num);

	int ApplyEquipmentAttack(int attack);
	int ApplyEquipmentDefense(int defense);

	bool IsFull() const { return size_ >= kBackpackSlotNum; }

private:
	std::array<EquipmentPrototype*, kEquipmentSlotNum> equipments_ { nullptr };
	std::array<ItemPrototype*, kBackpackSlotNum> items_ { nullptr };
	int size_ { 0 };
};
