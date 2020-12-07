#pragma once

#include <DrawSprites.hpp>

#include <unordered_map>
#include <string>
#include <vector>

class Player;

class ItemPrototype {
public:
	static ItemPrototype* GetPrototype(const std::string& name) { return prototypes_.at(name); }
	static ItemPrototype* GetPrototype(int idx) { return prototypes_.at(prototype_names_[idx]); }
	static size_t GetPrototypeNum() { return prototypes_.size(); }

	virtual const std::string& GetName() const = 0;
	virtual const std::string& GetDescription() const = 0;
	virtual const Sprite* GetIconSprite() const = 0;
	virtual const Sprite* GetPickupSprite() const = 0;
	virtual int GetPrice() const = 0;
	virtual void Apply(Player* player) = 0;
	
protected:
	static void AddPrototype(const std::string& name,  ItemPrototype* item_prototype) { 
		prototypes_[name] = item_prototype;
		prototype_names_.push_back(name);
	}

private:
	static std::unordered_map<std::string, ItemPrototype*> prototypes_;
	static std::vector<std::string> prototype_names_;
};
