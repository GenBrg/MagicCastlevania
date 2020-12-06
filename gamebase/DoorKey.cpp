#include "DoorKey.hpp"

#include <main_play.hpp>

const Sprite* DoorKey::sprite_;

DoorKey::DoorKey(Room& room, const glm::vec2& spawn_position) :
Entity(glm::vec4(spawn_position, spawn_position + sprite_->size_px), nullptr)
{
	transform_.position_ = spawn_position;
	trigger_ = Trigger::Create(room, glm::vec4(spawn_position, spawn_position + sprite_->size_px), nullptr, 1);
	trigger_->SetOnEnter([&](){
		main_play->CollectKey();
		Destroy();
	});
}

void DoorKey::DrawImpl(DrawSprites& draw)
{
	draw.draw(*sprite_, transform_);
}
