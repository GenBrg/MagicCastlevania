#pragma once

#include "glm/glm.hpp"
#include "Animation.hpp"
#include "Collider.hpp"

class AOE {
private:
	glm::vec2 position;
	glm::vec2 scale;
	glm::vec2 velocity;

	Animation animation;
	Collider collider;
};