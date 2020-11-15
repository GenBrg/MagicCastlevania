#pragma once

#include "../DrawSprites.hpp"
#include "Entity.hpp"
#include "CollisionSystem.hpp"

#include <vector>

class World {
public:
	void Update(float elapsed);
	void Draw(DrawSprites& draw);
	virtual ~World();

	void AddEntity(Entity* entity) { entities_.push_back(entity); }

	World() {}

protected:
	std::vector<Entity*> entities_;
	CollisionSystem collision_system_;
	
	virtual void UpdateImpl(float elapsed) = 0;
	virtual void DrawImpl(DrawSprites& draw) = 0;
};