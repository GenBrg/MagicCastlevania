#include "World.hpp"

void World::Update(float elapsed)
{
	// Update entities and garbage collection
	entities_.erase(std::remove_if(entities_.begin(), entities_.end(), [&](Entity* entity){
		entity->Update(elapsed);
		if (entity->IsDestroyed()) {
			delete entity;
			return true;
		}
		return false;
	}), entities_.end());

	collision_system_.Update(elasped);

	UpdateImpl(elapsed);
}

void World::Draw(DrawSprites& draw)
{
	for (const auto& entity : entities_) {
		entity->Draw(draw);
	}

	DrawImpl(draw);
}