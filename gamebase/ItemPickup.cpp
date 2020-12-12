#include "ItemPickup.hpp"

#include <engine/Trigger.hpp>
#include <gamebase/Room.hpp>
#include <gamebase/Player.hpp>
#include <gamebase/ItemPrototype.hpp>
#include <Util.hpp>

#include <iostream>

ItemPickUp::ItemPickUp(ItemPrototype* item, const glm::vec2& spawn_pos) :
Entity(glm::vec4(item->GetPickupSprite()->min_px - item->GetPickupSprite()->anchor_px,
 item->GetPickupSprite()->max_px - item->GetPickupSprite()->anchor_px),
nullptr),
item_prototype_(item)
{
	transform_.position_ = spawn_pos;
}

ItemPickUp* ItemPickUp::Generate(Room& room, ItemPrototype* item, const glm::vec2& spawn_pos, bool permanent)
{
	ItemPickUp* item_pickup = new ItemPickUp(item, spawn_pos);
	if (!permanent) {
		room.AddItem(item_pickup);
	} else {
		room.AddPermanentItem(item_pickup);
	}
	
	const Sprite* item_sprite = item->GetPickupSprite();
	glm::vec2 anchor_offset = item_sprite->min_px - item_sprite->anchor_px;
	item_pickup->trigger_ = Trigger::Create(room, glm::vec4(spawn_pos + anchor_offset, spawn_pos + anchor_offset + item_sprite->size_px), nullptr, 0);
	item_pickup->trigger_->SetOnColliding([=](){
		if (player->PickupItem(item)) {
			Sound::play(*sound_samples["collect_item"]);
			item_pickup->Destroy();
		}
	});
	return item_pickup;
}

void ItemPickUp::DrawImpl(DrawSprites& draw)
{
	draw.draw(*(item_prototype_->GetPickupSprite()), transform_);
}

void ItemPickUp::Destroy()
{
	destroyed_ = true;
	trigger_->Destroy();
}
