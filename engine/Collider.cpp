#include "Collider.hpp"

#include <algorithm>

Collider::Collider(const glm::vec4& box):
upper_right_corner_ { box[0], box[1] },
lower_left_corner_ { box[2], box[3] }
{}

// bool Collider::isCollide(const Collider& other)
// {
	
// }

// bool Collider::DynamicCollisionResolution(const Collider& other)
// {

// }

// bool Collider::IsPointInCollider(const glm::vec2& point) const
// {
// 	return point.x >= box_.x && point.x <= box_.z 
// 		&& point.y >= box_.y && point.y <= box_.w;
// }

bool Collider::IsIntersectWithRay(const glm::vec2& ray_origin, const glm::vec2& ray_dir, 
					glm::vec2& contact_point, glm::vec2& contact_normal, float& t_hit_near) const
{
	// TODO Edge cases: vertical and horizontal lines.

	glm::vec2 t_near = (upper_right_corner_ - ray_origin) / ray_dir;
	glm::vec2 t_far = (lower_left_corner_ - ray_origin) / ray_dir;

	if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
	if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

	if (t_near.x > t_far.y || t_near.y > t_far.x) return false;

	t_hit_near = std::max(t_near.x, t_near.y);
	float t_hit_far = std::min(t_far.x, t_far.y);

	if (t_hit_far < 0.0f) return false;

	contact_point = ray_origin + t_hit_near * ray_dir;

	if (t_near.x > t_near.y) {
		contact_normal = (ray_dir.x > 0.0f) ? { -1.0f, 0.0f } : { 1.0f, 0.0f };
	} else {
		contact_normal = (ray_dir.y > 0.0f) ? { 0.0f, -1.0f } : { 0.0f, 1.0f };
	}

	return true;
}
