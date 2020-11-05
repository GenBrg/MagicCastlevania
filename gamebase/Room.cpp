
#include "Room.hpp"
#include "../Util.hpp"
#include <fstream>
#include <iostream>

/**
 * Constructor of a room
 * @param platform_file the position of platforms in this room
 */
Room::Room(const std::string& platform_file) {
	std::ifstream in_file(platform_file);
	float top_left_x, top_left_y, relative_x, relative_y;
	std::string platform_name;

	while (in_file >> top_left_x >> top_left_y >> relative_x >> relative_y >> platform_name) {
		glm::vec4 bounding_box(top_left_x, VIEW_MAX.y - top_left_y - relative_y,
						 top_left_x + relative_x,VIEW_MAX.y - top_left_y);

		colliders.push_back(new Collider(bounding_box, nullptr));
	}
//	for(auto* p: colliders) {a
//		util::PrintVec4(p->GetBoundingBox());
//	}
	// tmp hard code to generate monsters
	monsters.push_back(new Monster(glm::vec2(505, 198), 60, "ghost_idle_1"));
}

Room::~Room() {
	for(auto* p: colliders) {
		free(p);
	}
}
