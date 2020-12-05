#include "MonsterAI.hpp"

#include <gamebase/Monster.hpp>
#include <gamebase/Player.hpp>
#include <engine/Random.hpp>

#include <stdexcept>
#include <iostream>

IMonsterAI* IMonsterAI::GetMonsterAI(Monster* monster, const json& j)
{
	int ai_type = j.at("type").get<int>();
	switch(ai_type) {
		case 0:
			return new BasicMovementMonsterAI(j, monster);
		break;
		case 1:
			return new BouncingMonsterAI(j, monster);
		break;
		case 2:
			return new FollowAndAttackMonsterAI(j, monster);
		break;
		case 3:
			return new RandomWalkingMonsterAI(j, monster);
		break;
		default:
		throw std::runtime_error("Can not recognize monster ai type: " + std::to_string(ai_type));
	}
}

BasicMovementMonsterAI::BasicMovementMonsterAI(const json& j, Monster* monster) :
transform_(monster->GetTransform()),
monster_(*monster)
{
	attack_cooldown_ = 1.0f + 3.0f * Random::Instance()->Generate();
}

void BasicMovementMonsterAI::Update(float elapsed)
{
	if (monster_.GetState() == Mob::State::MOVING) {
		float speed = monster_.GetSpeed();
		glm::vec2 central_pos = monster_.GetCentralPos();
		float move_radius = monster_.GetMoveRadius();

		transform_.position_.x += speed * elapsed * transform_.scale_.x;
		if (transform_.position_.x > central_pos.x + move_radius ||
			transform_.position_.x < central_pos.x - move_radius) {
			transform_.position_.x -= speed * elapsed * transform_.scale_.x;
			transform_.scale_.x *= -1;
		}
		
		if (monster_.GetAttackNum() > 0) {
			attack_cooldown_ -= elapsed;
			if (attack_cooldown_ <= 0.0f) {
				Attack* attack = monster_.GetAttack(0);
				monster_.PerformAttack(monster_.GetRoom(), *attack);
				attack_cooldown_ = (1 + Random::Instance()->Generate()) * attack->GetCoolDown();
			}
		}
	}
}

BouncingMonsterAI::BouncingMonsterAI(const json& j, Monster* monster) :
transform_(monster->GetTransform()),
monster_(*monster)
{
	GenerateSpeedVec();
	attack_cooldown_ = 1.0f + 3.0f * Random::Instance()->Generate();
}

void BouncingMonsterAI::Update(float elapsed)
{
	if (monster_.GetState() == Mob::State::MOVING) {
		bounce_cooldown_ -= elapsed;
		transform_.position_ += speed_vec_ * elapsed;
		if (transform_.position_.x <= 0.0f || transform_.position_.y <= 0.0f ||
		transform_.position_.x >= INIT_WINDOW_W || transform_.position_.y >= INIT_WINDOW_H ||
		bounce_cooldown_ <= 0.0f) {
			transform_.position_ -= speed_vec_ * elapsed;
			GenerateSpeedVec();
		}

		if (monster_.GetAttackNum() > 0) {
			attack_cooldown_ -= elapsed;
			if (attack_cooldown_ <= 0.0f) {
				Attack* attack = monster_.GetAttack(0);
				monster_.PerformAttack(monster_.GetRoom(), *attack);
				attack_cooldown_ = (1 + Random::Instance()->Generate()) * attack->GetCoolDown();
			}
		}
	}
}

void BouncingMonsterAI::GenerateSpeedVec()
{
	float angle = glm::radians(360.0f * Random::Instance()->Generate());
	speed_vec_ = monster_.GetSpeed() * glm::vec2(glm::cos(angle), glm::sin(angle));
	bounce_cooldown_ = 1.0f + 4.0f * Random::Instance()->Generate();
}

FollowAndAttackMonsterAI::FollowAndAttackMonsterAI(const json& j, Monster* monster) :
transform_(monster->GetTransform()),
monster_(*monster)
{
	assert(monster->GetAttackNum() > 0);

	glm::vec2 central_pos = monster->GetCentralPos();
	float move_radius = monster->GetMoveRadius();

	Attack* attack = monster->GetAttack(0);
	glm::vec4 aoe_bounding_box = attack->GetAOEPrototype()->GetBoundingBox();
	glm::vec2 aoe_initial_pos = attack->GetAOEPrototype()->GetInitialOffset();
	float aoe_width = aoe_bounding_box[2] - aoe_bounding_box[0];
	float aoe_height = aoe_bounding_box[3] - aoe_bounding_box[1];
	glm::vec4 detection_bounding_box;
	if (attack->IsAttachToEntity()) {
		detection_bounding_box[1] = central_pos[1] + aoe_initial_pos[1];
		detection_bounding_box[3] = central_pos[1] + aoe_initial_pos[1] + aoe_height;
		detection_bounding_box[0] = central_pos[0] - move_radius - aoe_initial_pos[0] - aoe_width;
		detection_bounding_box[2] = central_pos[0] + move_radius + aoe_initial_pos[0] + aoe_width;
		attack_trigger_ = Trigger::Create(monster->GetRoom(), aoe_bounding_box + glm::vec4(aoe_initial_pos, aoe_initial_pos), &transform_, 0);
	} else {
		detection_bounding_box[1] = central_pos[1] + aoe_initial_pos[1];
		detection_bounding_box[3] = central_pos[1] + aoe_initial_pos[1] + aoe_height;
		detection_bounding_box[0] = 0;
		detection_bounding_box[2] = INIT_WINDOW_W;
		attack_trigger_ = Trigger::Create(monster->GetRoom(), detection_bounding_box, nullptr, 0);
	}
	detection_trigger_ = Trigger::Create(monster->GetRoom(), detection_bounding_box, nullptr, 0);

	// TODO Implement trigger
	attack_trigger_->SetOnColliding([](){
		std::cout << "Monster attack!" << std::endl;
	});
	detection_trigger_->SetOnColliding([](){
		std::cout << "Monster detected player!" << std::endl;
	});
}

void FollowAndAttackMonsterAI::Update(float elapsed)
{
	
}

FollowAndAttackMonsterAI::~FollowAndAttackMonsterAI()
{

}

RandomWalkingMonsterAI::RandomWalkingMonsterAI(const json& j, Monster* monster) :
transform_(monster->GetTransform()),
monster_(*monster)
{
	GenerateTargetPos();
	attack_cooldown_ = 1.0f + 3.0f * Random::Instance()->Generate();
}

void RandomWalkingMonsterAI::Update(float elapsed)
{
	if (monster_.GetState() == Mob::State::MOVING) {
		// Face player
		transform_.scale_.x = (player->GetTransform().position_.x > transform_.position_.x) ? 1.0f : -1.0f;

		float speed = monster_.GetSpeed();

		if (target_pos_.x > transform_.position_.x) {
			transform_.position_.x += speed * elapsed;
			if (transform_.position_.x > target_pos_.x) {
				transform_.position_.x = target_pos_.x;
				GenerateTargetPos();
			}
		} else {
			transform_.position_.x -= speed * elapsed;
			if (transform_.position_.x < target_pos_.x) {
				transform_.position_.x = target_pos_.x;
				GenerateTargetPos();
			}
		}

		if (monster_.GetAttackNum() > 0) {
			attack_cooldown_ -= elapsed;
			if (attack_cooldown_ <= 0.0f) {
				Attack* attack = monster_.GetAttack(0);
				monster_.PerformAttack(monster_.GetRoom(), *attack);
				attack_cooldown_ = (1 + Random::Instance()->Generate()) * attack->GetCoolDown();
			}
		}
	}
}

void RandomWalkingMonsterAI::GenerateTargetPos()
{
	target_pos_ = monster_.GetCentralPos();
	float move_radius = monster_.GetMoveRadius();

	target_pos_.x += move_radius * (2 * Random::Instance()->Generate() - 1);
}
