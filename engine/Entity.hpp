#pragma once

#include "Transform2D.hpp"
#include "Collider.hpp"
#include "../DrawSprites.hpp"


class Entity {
public:
	void Update(float elapsed);
	void Draw(DrawSprites& draw);

	virtual void Destroy() { destroyed_ = true; }
	virtual bool IsDestroyed() const { return destroyed_; }
	Collider* GetCollider() { return &collider_; }
	Transform2D& GetTransform() { return transform_; }
	
	virtual ~Entity() {}

protected:
	Transform2D transform_;
	Collider collider_;

	bool destroyed_ { false };

	Entity(const glm::vec4& bounding_box, Transform2D* transform):
	transform_(transform),
	collider_(bounding_box, &transform_)
	 { }

	virtual void UpdateImpl(float elapsed) = 0;
	virtual void DrawImpl(DrawSprites& draw) = 0;
};
