#pragma once

#include <DrawSprites.hpp>

#include <unordered_map>
#include <string>

class Player;

class ItemPrototype {
public:
	static ItemPrototype* GetPrototype(const std::string& name) { return prototypes_.at(name); }

	virtual const std::string& GetName() const = 0;
	virtual const std::string& GetDescription() const = 0;
	virtual const Sprite* GetIconSprite() const = 0;
	virtual const Sprite* GetPickupSprite() const = 0;
	virtual void Apply(Player* player) = 0;
	
protected:
	static void AddPrototype(const std::string& name,  ItemPrototype* item_prototype) { 
		prototypes_[name] = item_prototype; 
	}

private:
	static std::unordered_map<std::string, ItemPrototype*> prototypes_;
};
