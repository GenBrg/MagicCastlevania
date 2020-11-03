#pragma once

#include <glm/glm.hpp>

class Collider {
private:
	//box
	glm::vec4 box;

public:
	bool isCollide(Collider other);
};