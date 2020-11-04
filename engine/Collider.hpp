#pragma once

#include "CollisionSystem.hpp"
#include "Transform2D.hpp"

#include <glm/glm.hpp>

/**
 * An axis-aligned bounding bounding_box that is able to do collision detection and resolution.
 * It uses swept AABB collision detection and resolution.
 * Idea from https://www.youtube.com/watch?v=8JJ-4JgR7Dg&t=24s&ab_channel=javidx9
 * Code reference from https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/Videos/OneLoneCoder_PGE_Rectangles.cpp
 * @author Jiasheng Zhou
 */
class Collider {
	friend class CollisionSystem;
public:
	/**
	 * @param bounding_box (xmin, ymin, xmax, ymax). Relative position to the anchor point.
	 * @param transform The parent transform of this collider which represents the anchor
	 * 					point . If nullptr is passed in, the bounding_box will essentially be an absolute position.
	 * @note The bounding_box is the position relative to the anchor point rather than an absolute position.
	 */
	Collider(const glm::vec4& bounding_box, Transform2D* transform);

	/** Naively statically check if this collider is colliding with another collider.
	 *  @param other_collider The collider to check with.
	 *  @return True if the a collision is detected.
	 */
	bool IsColliding(const Collider& other_collider);

	/** Swept AABB collision detection and resolution.
	 *  @param other_collider The collider to check with.
	 *  @param delta_position The offset of for the collider to advance.
	 *  @return True if the a collision is detected.
	 */
	bool DynamicCollisionQuery(const Collider& other_collider, const glm::vec2& delta_position, glm::vec2& contact_point, glm::vec2& contact_normal, float& time);

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

	void GetCorners(glm::vec2& upper_right_corner, glm::vec2& lower_left_corner);

private:
	glm::vec4 bounding_box_;
	Transform2D transform_;
};
