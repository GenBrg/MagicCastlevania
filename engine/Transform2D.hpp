#pragma once

#include <glm/glm.hpp>

/**
 * 2D Transformation
 * @author Jiasheng ZHou
 */
struct Transform2D {
	Transform2D* parent_ { nullptr };
	glm::vec2 position_ { 0.0f, 0.0f };
	float rotation_ { 0.0f };
	glm::vec2 scale_ { 1.0f, 1.0f };

	Transform2D(Transform2D* parent) :
	parent_(parent)
	{}

	glm::mat3 MakeLocalToParent() const;
	glm::mat3 MakeParentToLocal() const;
	
	glm::mat3 MakeLocalToWorld() const;
	glm::mat3 MakeWorldToLocal() const;

	glm::mat3 GetTranslationMat() const;
	glm::mat3 GetRotationMat() const;
	glm::mat3 GetScaleMat() const;

	glm::mat3 GetTranslationMat_r() const;

	//Transform2D(Transform2D const &) = delete;
};
