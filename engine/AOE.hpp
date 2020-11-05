// #pragma once

// #include "glm/glm.hpp"
// #include "Animation.hpp"
// #include "Collider.hpp"

// class AOE {
// private:
// 	glm::vec2 position;
// 	glm::vec2 scale;
// 	glm::vec2 velocity;

// 	Animation animation;
// 	Collider collider;
// };

#pragma once

#include "Transform2D.hpp"
#include "Collider.hpp"

class AOE {
public:
	

private:
	Transform2D transform_;
	Collider collider_;
	int attack_;
	
};