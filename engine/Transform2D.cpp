#include "Transform2D.hpp"
#include <iostream>

glm::mat3 Transform2D::MakeLocalToParent() const
{
	return GetTranslationMat() * GetRotationMat() * GetScaleMat();
}

glm::mat3 Transform2D::MakeParentToLocal() const
{
	return glm::inverse(MakeLocalToParent());
}

glm::mat3 Transform2D::MakeLocalToWorld() const
{
	if (!parent_) {
		return MakeLocalToParent();
	}
	return parent_->MakeLocalToWorld() * MakeLocalToParent();
}

glm::mat3 Transform2D::MakeWorldToLocal() const
{
	if (!parent_) {
		return MakeParentToLocal();
	}

	return MakeParentToLocal() * parent_->MakeWorldToLocal();
}

glm::mat3 Transform2D::GetTranslationMat() const
{
	glm::mat3 translation_mat(1.0f);

	translation_mat[2] = glm::vec3(position_, 1.0f);

	return translation_mat;
}

glm::mat3 Transform2D::GetRotationMat() const
{
	glm::mat3 rotation_mat(1.0f);

	float cos_theta = glm::cos(rotation_);
	float sin_theta = glm::sin(rotation_);

	rotation_mat[0] = glm::vec3(cos_theta, sin_theta, 0.0f);
	rotation_mat[1] = glm::vec3(-sin_theta, cos_theta, 0.0f);

	return rotation_mat;
}

glm::mat3 Transform2D::GetScaleMat() const
{
	glm::mat3 scale_mat(1.0f);

	scale_mat[0][0] = scale_[0];
	scale_mat[1][1] = scale_[1];

	return scale_mat;
}
