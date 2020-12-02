#pragma once

#include <gamebase/ItemPrototype.hpp>
#include <gamebase/Buff.hpp>
#include <DrawSprites.hpp>

#include <string>
#include <vector>

class EquipmentPrototype : public ItemPrototype {
public:
	static void LoadConfig(const std::string& equipment_list_file);

	virtual const std::string& GetName() const { return name_; }
	virtual const std::string& GetDescription() const { return description_; }
	virtual const Sprite* GetIconSprite() const { return icon_sprite_; }
	virtual const Sprite* GetPickupSprite() const { return pickup_sprite_; }

	virtual void Apply(Player* player);
	int ApplyAttack(int attack);
	int ApplyDefense(int defense);

private:
	EquipmentPrototype() {}

	std::string name_;
	std::string description_;
	const Sprite* icon_sprite_;
	const Sprite* pickup_sprite_;

	int attack_;
	int defense_;
	int slot_;
};
