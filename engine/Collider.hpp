#pragma once

#include "CollisionSystem.hpp"
#include "Transform2D.hpp"

#include <glm/glm.hpp>

/**
 * An axis-aligned bounding box that is able to do collision detection and resolution.
 * It uses swept AABB collision detection and resolution.
 * Idea from https://www.youtube.com/watch?v=8JJ-4JgR7Dg&t=24s&ab_channel=javidx9
 * Code reference from https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/Videos/OneLoneCoder_PGE_Rectangles.cpp
 * @author Jiasheng Zhou
 */
class Collider {
	friend class CollisionSystem;
public:
	/**
	 * @param box (xmin, ymin, xmax, ymax).
	 */
	Collider(const glm::vec4& box);

	/** Naively statically check if this collider is colliding with another collider.
	 *  @param other The collider to check with.
	 *  @return True if the a collision is detected.
	 */
	bool IsCollide(const Collider& other);

	/** Swept AABB collision detection and resolution.
	 *  @param other The collider to check with.
	 *  @return True if the a collision is detected.
	 */
	bool DynamicCollisionResolution(const Collider& other);

	bool IsPointInCollider(const glm::vec2& point) const;

	/** Check if a ray intersects with the collider.
	 *  @param ray_origin The origin of the ray.
	 *  @param ray_dir The direction vector of the ray.
	 *  @param contact_point The contact point of the near intersection of the ray and the collider.
	 *  @param contact_normal The contact normal of the near intersection of the ray and the collider.
	 *  @param hit_near The time(distance) from the ray origin to the contact point.
	 *  @return True if the ray intersects with the collider.
	 */
	bool IsIntersectWithRay(const glm::vec2& ray_origin, const glm::vec2& ray_dir,
	glm::vec2& contact_point, glm::vec2& contact_normal, float& t_hit_near) const;



private:
	//box
	glm::vec2 upper_right_corner_;
	glm::vec2 lower_left_corner_;

	Transform2D* transform_;
	glm::vec2 last_position_;
};
