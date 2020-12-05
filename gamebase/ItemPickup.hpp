#pragma once

#include <engine/Entity.hpp>

class ItemPrototype;
class Trigger;
class Room;

class ItemPickUp : public Entity {
public:
	static ItemPickUp* Generate(Room& room, ItemPrototype* item, const glm::vec2& spawn_pos, bool permanent = false);
	virtual void Destroy();

private:
	Trigger* trigger_;
	ItemPrototype* item_prototype_;

	ItemPickUp(ItemPrototype* item, const glm::vec2& spawn_pos);	
	virtual void UpdateImpl(float elapsed) {}
	virtual void DrawImpl(DrawSprites& draw);
};
