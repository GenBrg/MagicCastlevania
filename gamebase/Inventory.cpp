#include "Inventory.hpp"

bool Inventory::PushItem(ItemPrototype* item)
{
	if (IsFull()) {
		return false;
	}

	for (auto& slot : items_) {
		if (!slot) {
			slot = item;
			++size_;
			return true;
		}
	}
	return false;
}

ItemPrototype* Inventory::PeekItem(size_t slot_num)
{
	return items_[slot_num];
}

EquipmentPrototype* Inventory::PeekEquipment(size_t slot_num)
{
	return equipments_[slot_num];
}

ItemPrototype* Inventory::PopItem(size_t slot_num)
{
		ItemPrototype* item = items_[slot_num];
		if (item) {
			--size_;
		}
		items_[slot_num] = nullptr;
		return item;
		
}

EquipmentPrototype* Inventory::PopEquipment(size_t slot_num)
{
	EquipmentPrototype* equipment = equipments_[slot_num];
	equipments_[slot_num] = nullptr;
	return equipment;
}

void Inventory::UseItem(Player* player, size_t slot_num)
{
	ItemPrototype* item = PopItem(slot_num);
	if (item) {
		item->Apply(player);
		if (EquipmentPrototype* equipment = dynamic_cast<EquipmentPrototype*>(item)) {
			int equip_slot = equipment->GetSlotNum();
			EquipmentPrototype* orig_equipment = PopEquipment(equip_slot);
			if (orig_equipment) {
				PushItem(orig_equipment);
			}
			equipments_[equip_slot] = equipment;
		}
	}
}

void Inventory::UnequipItem(size_t slot_num)
{
	if (!IsFull()) {
		EquipmentPrototype* equipment = PopEquipment(slot_num);
		if (equipment) {
			PushItem(equipment);
		}
	}
}

int Inventory::ApplyEquipmentAttack(int attack)
{
	for (const auto& equipment : equipments_) {
		if (equipment) {
			attack += equipment->ApplyAttack(attack);
		}
	}
	return attack;
}

int Inventory::ApplyEquipmentDefense(int defense)
{
	for (const auto& equipment : equipments_) {
		if (equipment) {
			defense += equipment->ApplyDefense(defense);
		}
	}
	return defense;
}

const Sprite* Inventory::GetItemIconSprite(size_t slot_num)
{
	if (items_[slot_num]) {
		return items_[slot_num]->GetIconSprite();
	}
	return nullptr;
}

const Sprite* Inventory::GetEuqipmentIconSprite(size_t slot_num)
{
	if (equipments_[slot_num]) {
		return equipments_[slot_num]->GetIconSprite();
	}
	return nullptr;
}
