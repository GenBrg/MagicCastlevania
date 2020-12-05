#include "MonsterAI.hpp"

#include <gamebase/Monster.hpp>

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
{}

void BasicMovementMonsterAI::Update(float elapsed)
{

	float speed = monster_.GetSpeed();
	glm::vec2 central_pos = monster_.GetCentralPos();
	float move_radius = monster_.GetMoveRadius();

	if (monster_.GetState() == Mob::State::MOVING) {
		transform_.position_.x += speed * elapsed * transform_.scale_.x;
		if (transform_.position_.x > central_pos.x + move_radius ||
			transform_.position_.x < central_pos.x - move_radius) {
			transform_.position_.x -= speed * elapsed * transform_.scale_.x;
			transform_.scale_.x *= -1;
		}
	}
}

BouncingMonsterAI::BouncingMonsterAI(const json& j, Monster* monster) :
transform_(monster->GetTransform()),
monster_(*monster)
{}

void BouncingMonsterAI::Update(float elapsed)
{
	
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

RandomWalkingMonsterAI::RandomWalkingMonsterAI(const json& j, Monster* monster) :
transform_(monster->GetTransform()),
monster_(*monster)
{}

void RandomWalkingMonsterAI::Update(float elapsed)
{
	
}

FollowAndAttackMonsterAI::~FollowAndAttackMonsterAI()
{

}
