// #pragma once

// #include "Collider.hpp"
// #include "Transform2D.hpp"

// #include <glm/glm.hpp>

// #include <cstdint>
// #include <vector>

// class CollisionSystem {
// public:
// 	enum class Channel : uint8_t {
// 		DEFAULT = 0,
// 		MAP,
// 		MONSTER,
// 		PLAYER,
// 		ITEM,
// 		AOE,
// 		MAX
// 	};

// 	CollisionSystem() : 
// 	colliders_(static_cast<size_t>(Channel::MAX), std::vector<Collider>())
// 	{}

// 	// Factory
// 	Collider* GetCollider(Channel channel, Transform2D* transform2D, const glm::vec4& box);
// 	void Update(float elapsed);

// private:
// 	std::vector<std::vector<Collider>> colliders_;
// };