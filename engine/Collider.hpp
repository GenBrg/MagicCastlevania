#pragma once

#include <engine/CollisionSystem.hpp>
#include <engine/Transform2D.hpp>
#include <DrawSprites.hpp>

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
	 *  @param contact_point The center position of the bounding box when the collide happens.
	 *  @param contact_normal The contact normal of the collision surface.
	 *  @param time The time(distance) from the bounding box center to contact_point.
	 *  @return True if the a collision is detected.
	 */
	bool DynamicCollisionQuery(const Collider& other_collider, const glm::vec2& delta_position, glm::vec2& contact_point, glm::vec2& contact_normal, float& time);

	/** Get the absolute position of the bounding box after transformation.
	 *  @param lower_left_corner The lower left corner position of the bounding box.
	 *  @param upper_right_corner The upper right corner position of the bounding box.
	 */
	void GetCorners(glm::vec2& lower_left_corner, glm::vec2& upper_right_corner) const;

	/** Get the relative bounding box to the anchor point.
	 *  @return Relative bounding box to the anchor point.
	 */
	glm::vec4 GetBoundingBox() const;

	void DrawDebugBox(DrawSprites& draw) const;

	/** Set the relative bounding box to the anchor point.
	 * @param Relative bounding box to the anchor point.
	 */
	void SetBoundingBox(const glm::vec4& bounding_box) { bounding_box_ = bounding_box; }  

	/** Set if the collider can only detect dynamic collision from above to below.
	 *  @param one_sided True if the collider can only detect dynamic collision from above to below.
	 */
	void SetOneSided(bool one_sided) { one_sided_ = one_sided; }

private:
	glm::vec4 bounding_box_;
	Transform2D transform_;
	bool one_sided_ { false };
};
