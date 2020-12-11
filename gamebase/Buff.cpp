#include "Buff.hpp"

bool Buff::Update(float elapsed)
{
	duration_ -= elapsed;
	return duration_ <= 0.0f;
}

void Buff::Clear()
{
	attack_ = 0;
	defense_ = 0;
	duration_ = 0.0f;
}

void from_json(const json& j, Buff& buff)
{
	buff.attack_ = j.at("attack").get<int>();
	buff.defense_ = j.at("defense").get<int>();
	buff.duration_ = j.at("duration").get<float>();
	json icon_json = j.at("icon_sprite");
	if (!icon_json.is_null()) {
		buff.icon_sprite_ = &(sprites->lookup(icon_json.get<std::string>()));
	} else {
		buff.icon_sprite_ = nullptr;
	}
}
