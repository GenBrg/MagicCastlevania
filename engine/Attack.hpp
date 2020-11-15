#pragma once

#include <engine/AOEPrototype.hpp>
#include <engine/TimerGuard.hpp>
#include <engine/Transform2D.hpp>
#include <gamebase/Room.hpp>
#include <Util.hpp>

class Attack {
public:
	Attack() = default;
	Attack(Animation* mob_animation, AOEPrototype* aoe_prototype, float cooldown, bool is_monster, bool attach_to_entity) :
	mob_animation_(mob_animation),
	aoe_prototype_(aoe_prototype),
	cooldown_(cooldown),
	is_monster_(is_monster),
	attach_to_entity_(attach_to_entity)
	{}

	bool Execute(Room& room, int attack, Transform2D& transform);
	Animation* GetAnimation() const { return mob_animation_; }

	friend void from_json(const json& j, Attack& attack);

private:
	Animation* mob_animation_;
	AOEPrototype* aoe_prototype_;
	float cooldown_;
	bool is_monster_; // TODO Bad
	bool attach_to_entity_;

	TimerGuard guard_;
};
