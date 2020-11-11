#include "Entity.hpp"

void Entity::Update(float elapsed)
{
	if (IsDestroyed()) {
		return;
	}

	UpdateImpl(elapsed);
}

void Entity::Draw(DrawSprites& draw)
{
	if (IsDestroyed()) {
		return;
	}

	DrawImpl(draw);
}
