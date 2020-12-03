#include "EquipmentPrototype.hpp"

#include <gamebase/Player.hpp>

void EquipmentPrototype::Apply(Player* player)
{

}

int EquipmentPrototype::ApplyAttack(int attack)
{
	return attack + attack_;
}

int EquipmentPrototype::ApplyDefense(int defense)
{
	return defense + defense_;
}
