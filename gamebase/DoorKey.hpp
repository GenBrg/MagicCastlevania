#pragma once

#include <engine/Entity.hpp>
#include <engine/Trigger.hpp>
#include <DrawSprites.hpp>

#include <glm/glm.hpp>

class DoorKey : public Entity
{
public:
	const static Sprite* sprite_;
	DoorKey(Room& room, const glm::vec2& spawn_position);

protected:
	virtual void UpdateImpl(float elapsed) {}
	virtual void DrawImpl(DrawSprites& draw);

private:
	Trigger* trigger_;
};
