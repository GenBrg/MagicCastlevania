#include "Collider.hpp"

#include <algorithm>

#include <iostream>
#include <Util.hpp>

Collider::Collider(const glm::vec4 &bounding_box, Transform2D *transform) : bounding_box_(bounding_box),
																			transform_(transform)
{
}

bool Collider::IsColliding(const Collider &other_collider)
{
	auto is_line_intersect = [](float x1, float y1, float x2, float y2) {
		return x1 < y2 && x2 < y1;
	};

	glm::vec2 lower_left_corner, upper_right_corner;
	glm::vec2 other_lower_left_corner, other_upper_right_corner;

	GetCorners(lower_left_corner, upper_right_corner);
	other_collider.GetCorners(other_lower_left_corner, other_upper_right_corner);

	return is_line_intersect(lower_left_corner.x, upper_right_corner.x, other_lower_left_corner.x, other_upper_right_corner.x) && is_line_intersect(lower_left_corner.y, upper_right_corner.y, other_lower_left_corner.y, other_upper_right_corner.y);
}

bool Collider::DynamicCollisionQuery(const Collider &other_collider, const glm::vec2 &delta_position, glm::vec2 &contact_point, glm::vec2 &contact_normal, float &time)
{
	if (delta_position.x == 0.0f && delta_position.y == 0.0f)
	{
		return false;
	}

	glm::vec2 lower_left_corner, upper_right_corner;
	glm::vec2 other_lower_left_corner, other_upper_right_corner;

	GetCorners(lower_left_corner, upper_right_corner);
	other_collider.GetCorners(other_lower_left_corner, other_upper_right_corner);

	glm::vec2 center = (lower_left_corner + upper_right_corner) / 2.0f;
	glm::vec2 half_box_size = (upper_right_corner - lower_left_corner) / 2.0f;

	// Extend the region of the other collider
	other_lower_left_corner -= half_box_size;
	other_upper_right_corner += half_box_size;

	contact_normal = glm::vec2(0.0f, 0.0f);

	auto ray_rect_intersection_test = [&](const glm::vec2 &ray_origin, const glm::vec2 &ray_dir, const glm::vec2 &lower_left_corner, const glm::vec2 &upper_right_corner,
										 glm::vec2 &contact_point, glm::vec2 &contact_normal, float &t_hit_near) {
		glm::vec2 t_near = (upper_right_corner - ray_origin) / ray_dir;
		glm::vec2 t_far = (lower_left_corner - ray_origin) / ray_dir;

		// TODO Edge cases: vertical and horizontal lines.

		if (std::isnan(t_near.x) || std::isnan(t_near.y))
		{
			return false;
		}
		if (std::isnan(t_far.x) || std::isnan(t_far.y))
		{
			return false;
		}

		if (t_near.x > t_far.x)
			std::swap(t_near.x, t_far.x);
		if (t_near.y > t_far.y)
			std::swap(t_near.y, t_far.y);

		if (t_near.x > t_far.y || t_near.y > t_far.x)
			return false;

		t_hit_near = std::max(t_near.x, t_near.y);
		float t_hit_far = std::min(t_far.x, t_far.y);

		if (t_hit_far < 0.0f)
			return false;

		contact_point = ray_origin + t_hit_near * ray_dir;

		if (t_near.x > t_near.y)
		{
			contact_normal = (ray_dir.x > 0.0f) ? glm::vec2(-1.0f, 0.0f) : glm::vec2(1.0f, 0.0f);
		}
		else
		{
			contact_normal = (ray_dir.y > 0.0f) ? glm::vec2(0.0f, -1.0f) : glm::vec2(0.0f, 1.0f);
		}

		if (other_collider.one_sided_ && contact_normal.y <= 0.0f)
		{
			return false;
		}

		return true;
	};

	return ray_rect_intersection_test(center, delta_position, other_lower_left_corner, other_upper_right_corner, contact_point, contact_normal, time) && time <= 1.0f && time >= -1.0f;
}

void Collider::GetCorners(glm::vec2 &lower_left_corner, glm::vec2 &upper_right_corner) const
{
	lower_left_corner = glm::vec2(bounding_box_[0], bounding_box_[1]);
	upper_right_corner = glm::vec2(bounding_box_[2], bounding_box_[3]);

	// glm::mat3 model_mat = transform_.MakeLocalToWorld();

	// Collider ignores scale and rotation because it only supports axis-aligned bounding box and sudden change may cause issue.
	glm::mat3 model_mat = transform_.GetTranslationMat_r();

	lower_left_corner = model_mat * glm::vec3(lower_left_corner, 1.0f);
	upper_right_corner = model_mat * glm::vec3(upper_right_corner, 1.0f);
}

glm::vec4 Collider::GetBoundingBox() const
{
	return bounding_box_;
}
