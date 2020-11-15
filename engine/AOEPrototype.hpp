#pragma once

#include "Transform2D.hpp"
#include "Animation.hpp"

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>

class AOE;
class Room;

/** The prototype of an AOE, typically owned by a Mob as a skill/attack.
 *  AOE is required to be instantiated from this class by providing a name.
 *  AOEPrototype can only be initialized from config files.
 */
class AOEPrototype {
public:
	/** Load the config file of a AOEPrototype and put it into the prototype collection.
	 *  @param config_name The name of the AOEprototype, the function will automatically find dist/config_name.AOE.
	 */
	static void LoadConfig(const std::string& config_file);
	static AOEPrototype* GetAOEPrototype(const std::string& name) { return &(prototypes_.at(name)); };

	/** Create an AOE in the room from the prototype.
	 * @param room The room the AOE spawns in.
	 * @param attack The attack point of this AOE.
	 * @param parent_transform The parent transform the AOE attaches to.
	 * @param is_monster Is the AOE created by a monster.
	 */
	AOE* Create(Room& room, int attack, Transform2D* parent_transform, bool is_monster, bool attach_to_entity);
	std::string to_string() const;

private:
	inline static std::unordered_map<std::string, AOEPrototype> prototypes_;

	/**
	 * @param bounding_box Bounding box of the AOE for collision detection.
	 * @param animation Animation of the AOE.
	 * @param velocity Velocity of the AOE.
	 * @param duration Duration of the AOE, for number less than 0.0f, it will remain forever.
	 * @param initial_offset The initial offset from the parent_transform or world origin.
	 * @param penetrate If the AOE can pierce through entities.
	 */
	AOEPrototype(const glm::vec4 bounding_box, Animation* animation, const glm::vec2 velocity,
	 float duration, const glm::vec2& initial_offset, bool penetrate) :
	bounding_box_(bounding_box),
	animation_(animation),
	velocity_(velocity),
	duration_(duration),
	initial_offset_(initial_offset),
	penetrate_(penetrate)
	{}

	glm::vec4 bounding_box_;
	Animation* animation_;
	glm::vec2 velocity_;
	float duration_;
	glm::vec2 initial_offset_;
	bool penetrate_;
};