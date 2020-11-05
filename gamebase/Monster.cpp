#include "../Util.hpp"
#include "../DrawSprites.hpp"
#include "Monster.hpp"

Monster::Monster(const glm::vec2& pos, float move_radius, std::string monster_key):
transform_(nullptr),
movement_component_(glm::vec4(0.0f, 0.0f, 51.0f, 66.0f), transform_),
sprite_(sprites->lookup(monster_key)),
move_radius_(move_radius)
{
	transform_.position_ = pos;
	central_pos_ = pos;
	mov_direction_ = 1;
}

void Monster::Draw(DrawSprites &draw) const {
	draw.draw(sprite_, transform_);
}

void Monster::Update(float elapsed, const std::vector<Collider*>& colliders_to_consider) {
	float move_speed = 50.0f;
	transform_.position_.x += move_speed * elapsed * (float)mov_direction_;
	if(transform_.position_.x > central_pos_.x + move_radius_ ||
		transform_.position_.x < central_pos_.x - move_radius_) {
		transform_.position_.x -= move_speed * elapsed * (float)mov_direction_;
		mov_direction_ *= -1;
	}
}

void Monster::TakeDamage(int attack)
{
	if (invulnerable_countdown_ <= 0.0f) {
		int damage = 1;

		if (attack > defense_) {
			damage = attack - defense_;
		}

		hp_ -= damage;

		invulnerable_countdown_ = kStiffnessTime;

		// if (hp_ <= 0) ...
	}
}
