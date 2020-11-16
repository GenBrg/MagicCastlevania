#include <Util.hpp>
#include <engine/AOEPrototype.hpp>
#include <gamebase/MonsterPrototype.hpp>
#include <data_path.hpp>
#include <iostream>

void TestLoadAnimation()
{
	Animation::LoadAnimation("../dist/animations.json");
	auto anim = Animation::GetAnimation("player_walk");
	std::cout << anim->GetLength() << std::endl;
}

void TestLoadAOEPrototype()
{
	AOEPrototype::LoadConfig("../dist/aoe_prototypes.json");
	auto p = AOEPrototype::GetAOEPrototype("normal_attack");
	std::cout << p->to_string() << std::endl;
}

void TestLoadMonster()
{
	MonsterPrototype::LoadConfig("../dist/monster.list");
	MonsterPrototype::GetMonsterPrototype("ghost");
}

int main()
{
	getchar();
	TestLoadAnimation();
	TestLoadAOEPrototype();
	TestLoadMonster();
	
	return 0;
}