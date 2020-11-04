#pragma once

#include <glm/glm.hpp>

struct Transform2D {
	glm::vec2 position_ { 0.0f, 0.0f };
	float rotation_ { 0.0f };
	glm::vec2 scale_ { 1.0f, 1.0f };
};
