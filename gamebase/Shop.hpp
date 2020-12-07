#pragma once

#include <gamebase/ItemPrototype.hpp>

#include <array>

class Shop {
public:
	inline static constexpr int kSlotNum = 12;
	Shop(glm::vec2 pos);
	void GenerateItems(size_t level);
	void Purchase(size_t slot_num);
	ItemPrototype* PeekItem(size_t slot_num) const { return items_[slot_num]; }
    void RegisterKeyEvents();
    void UnregisterKeyEvents();
    void Update(float elapsed);
    void Draw(DrawSprites& draw);

private:
	std::array<ItemPrototype*, kSlotNum> items_ { nullptr };
	glm::vec2 pos_;
    float elapsed_ {0.0f};
};
