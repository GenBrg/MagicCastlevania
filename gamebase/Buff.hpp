#pragma once

#include <DrawSprites.hpp>
#include <Util.hpp>

class Buff {
public:
	Buff(int attack, int defense, float duration) : 
	attack_(attack),
	defense_(defense),
	duration_(duration)
	{}

	Buff() = default;

	int ApplyAttack(int attack) const { return attack_ + attack; }
	int ApplyDefense(int defense) const { return defense_ + defense; }

	friend void from_json(const json& j, Buff& buff);

	/** Update the timer
	 *  @param elapsed
	 *  @return True if the buff expires
	 */
	bool Update(float elapsed);
	const Sprite* GetIconSprite() const { return icon_sprite_; }

private:
	int attack_;
	int defense_;
	float duration_;
	const Sprite* icon_sprite_;
};
