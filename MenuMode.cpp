#include "Mode.hpp"
#include "MenuMode.hpp"
#include <Util.hpp>

//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for easy sprite drawing:
#include "DrawSprites.hpp"

//for playing movement sounds:
#include "Sound.hpp"

//for loading:
#include "Load.hpp"
#include "data_path.hpp"
#include "main_play.hpp"
#include <random>
#include <stack>
//Load< Sound::Sample > sound_click(LoadTagDefault, []() -> Sound::Sample* {
//	std::vector< float > data(size_t(48000 * 0.2f), 0.0f);
//	for (uint32_t i = 0; i < data.size(); ++i) {
//		float t = i / float(48000);
//		//phase-modulated sine wave (creates some metal-like sound):
//		data[i] = std::sin(3.1415926f * 2.0f * 440.0f * t + std::sin(3.1415926f * 2.0f * 450.0f * t));
//		//quadratic falloff:
//		data[i] *= 0.3f * std::pow(std::max(0.0f, (1.0f - t / 0.2f)), 2.0f);
//	}
//	return new Sound::Sample(data);
//	});
//
//Load< Sound::Sample > sound_clonk(LoadTagDefault, []() -> Sound::Sample* {
//	std::vector< float > data(size_t(48000 * 0.2f), 0.0f);
//	for (uint32_t i = 0; i < data.size(); ++i) {
//		float t = i / float(48000);
//		//phase-modulated sine wave (creates some metal-like sound):
//		data[i] = std::sin(3.1415926f * 2.0f * 220.0f * t + std::sin(3.1415926f * 2.0f * 200.0f * t));
//		//quadratic falloff:
//		data[i] *= 0.3f * std::pow(std::max(0.0f, (1.0f - t / 0.2f)), 2.0f);
//	}
//	return new Sound::Sample(data);
//	});
# define MENU_FONT_PATH  "DigitalDisco.ttf"

MenuMode::MenuMode(std::vector< Item > const& items_, int width) : 
text(data_path(MENU_FONT_PATH)),
items(items_)
{
	//select first item which can be selected:
	text.SetFontSize(font_size)
		.SetColor(glm::u8vec4(0x00, 0x00, 0x00, 0xff))
		.SetPos(glm::vec2(117.0f, 328.0f));
	for (uint32_t i = 0; i < items.size(); ++i) {
		if (items[i].on_select) {
			selected = i;
			break;
		}
	}
	row_width = width;
}

MenuMode::~MenuMode() {
}

bool MenuMode::handle_event(SDL_Event const& evt, glm::uvec2 const& window_size) {
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_w) {
			//skip non-selectable items:
			for (int i = selected - row_width; i >= 0; --i) {
				if (items[i].on_select) {
					selected = i;
					Sound::play(*sound_samples["select"]);
					break;
				}
			}
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_s) {
			//note: skips non-selectable items:
			for (int i = selected + row_width; i < (int) items.size(); ++i) {
				if (items[i].on_select) {
					selected = i;
					Sound::play(*sound_samples["select"]);
					break;
				}
			}
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_a) {
			//note: skips non-selectable items:
			for (int i = selected - 1; i >= 0; --i) {
				if (items[i].on_select) {
					selected = i;
					Sound::play(*sound_samples["select"]);
					break;
				}
			}
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_d) {
			//note: skips non-selectable items:
			for (int i = selected + 1; i < (int) items.size(); ++i) {
				if (items[i].on_select) {
					selected = i;
					Sound::play(*sound_samples["select"]);
					break;
				}
			}
			return true;
		}
		else if (evt.key.keysym.sym == SDLK_SPACE) {
			if (selected < items.size() && items[selected].on_select) {
				//Sound::play(*sound_clonk);
				items[selected].on_select(items[selected]);
				return true;
			}
		}
		else if (row_width != 1) {
			if (evt.key.keysym.sym == SDLK_ESCAPE) {
				Mode::set_current(main_play);
				return true;
			}
			if (evt.key.keysym.sym == SDLK_j) {
				if (items[selected].on_discard) {
					items[selected].on_discard(items[selected]);
					return true;
				}
			}
		}
	}
	if (background) {
		return background->handle_event(evt, window_size);
	}
	else {
		return false;
	}
}

void MenuMode::update(float elapsed) {

	//select_bounce_acc = select_bounce_acc + elapsed / 0.7f;
	//select_bounce_acc -= std::floor(select_bounce_acc);

	if (background) {
		background->update(elapsed);
	}
	if (row_width == 4) {
		for (int i = 1; i < 5; i++) {
			items[i].item_prototype = player->GetEquipment(i - 1);
			if (items[i].item_prototype) {
				items[i].on_select = [=](MenuMode::Item const&) {
					player->UnequipItem(i - 1);
				};
				items[i].on_discard = [=](MenuMode::Item const&) {
					player->DropEquipment(i - 1);
				};
			}
			else {
				items[i].on_select = [=](MenuMode::Item const&) {
					
				};
				items[i].on_discard = [=](MenuMode::Item const&) {
					
				};
			}
		}
		for (int i = 5; i < 17; i++) {
			items[i].item_prototype = player->GetItem(i-5);
			if (items[i].item_prototype) {
				items[i].on_select = [=](MenuMode::Item const&) {
					player->UseItem(i-5);
				};
				items[i].on_discard = [=](MenuMode::Item const&) {
					player->DropItem(i-5);
				};
			}
			else {
				items[i].on_select = [=](MenuMode::Item const&) {

				};
				items[i].on_discard = [=](MenuMode::Item const&) {

				};
			}
		}
	}
}
void draw_digits(DrawSprites& draw_sprite, glm::vec2 pos, int val, glm::u8vec4 tint) {
	std::stack<int> digits;
	if (val == 0) {
		digits.push(0);
	}
	while (val > 0) {
		digits.push(val % 10);
		val = val / 10;
	}
	Transform2D transform(nullptr);
	transform.position_ = pos;

	while (!digits.empty()) {
		int digit = digits.top();
		digits.pop();
		auto digit_sprite = sprites->lookup("digit_" + std::to_string(digit));
		draw_sprite.draw(digit_sprite, transform, tint);
		transform.position_.x += digit_sprite.size_px.x;
	}
}
void MenuMode::draw(glm::uvec2 const& drawable_size) {
	if (background) {
		std::shared_ptr< Mode > hold_me = shared_from_this();
		background->draw(drawable_size);
		//it is an error to remove the last reference to this object in background->draw():
		assert(hold_me.use_count() > 1);
	}
	else {
		glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	//use alpha blending:
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//don't use the depth test:
	glDisable(GL_DEPTH_TEST);

	//float bounce = (0.25f - (select_bounce_acc - 0.5f) * (select_bounce_acc - 0.5f)) / 0.25f * select_bounce_amount;

	{ //draw the menu using DrawSprites:
		assert(atlas && "it is an error to try to draw a menu without an atlas");
		DrawSprites draw_sprites(*atlas, view_min, view_max, drawable_size, DrawSprites::AlignPixelPerfect);

		for (auto const& item : items) {
			bool is_selected = (&item == &items[0] + selected);
			// glm::u8vec4 color = (is_selected ? item.selected_tint : item.tint);
			//float left, right;
			if (item.item_prototype) {
				if (item.item_prototype->GetIconSprite()) {
					draw_sprites.draw(*item.item_prototype->GetIconSprite(), item.transform);
				}
				if (item.sprite_selected && is_selected) {
					draw_sprites.draw(*item.sprite_selected, item.transform);
				}
			}
			else {
				if (item.sprite) {
					draw_sprites.draw(*item.sprite, item.transform);
				}
				if (item.sprite_selected && is_selected) {
					draw_sprites.draw(*item.sprite_selected, item.transform);
				}
			}			
		}
		if (row_width == 4) {
			int hp_to_draw = player->GetHp();
			if (hp_to_draw > 0) {
				Transform2D hp_curr_transform = Transform2D(nullptr);
				hp_curr_transform.position_ = glm::vec2(194.0f, 406.0f);
				draw_sprites.draw(sprites->lookup("hp_corner1"), hp_curr_transform);
				hp_curr_transform.position_ += glm::vec2(4.0f, 0.0f);
				hp_to_draw -= 3;
				if (hp_to_draw >= 3) {
					for (; hp_to_draw >= 3; hp_to_draw--) {
						draw_sprites.draw(sprites->lookup("hp_point"), hp_curr_transform);
						hp_curr_transform.position_ += glm::vec2(1.0f, 0.0f);
					}
					draw_sprites.draw(sprites->lookup("hp_corner2"), hp_curr_transform);
				}
			}
			int exp_to_draw = player->GetCurLevelExp() * 80 /player->GetCurLevelMaxExp() ;
			if (exp_to_draw > 0) {
				Transform2D exp_curr_transform = Transform2D(nullptr);
				exp_curr_transform.position_ = glm::vec2(194.0f, 389.0f);
				draw_sprites.draw(sprites->lookup("exp_corner1"), exp_curr_transform);
				exp_curr_transform.position_ += glm::vec2(4.0f, 0.0f);
				exp_to_draw -= 3;
				if (exp_to_draw >= 3) {
					for (; exp_to_draw >= 3; exp_to_draw--) {
						draw_sprites.draw(sprites->lookup("exp_point"), exp_curr_transform);
						exp_curr_transform.position_ += glm::vec2(1.0f, 0.0f);
					}
					draw_sprites.draw(sprites->lookup("exp_corner2"), exp_curr_transform);
				}
			}
			draw_digits(draw_sprites, glm::vec2(232.0f, 365.0f), player->GetLevel(), glm::u8vec4(0x00, 0x00, 0x00, 0xff));
			draw_digits(draw_sprites, glm::vec2(309.0f, 365.0f), player->GetCoin(), glm::u8vec4(0x00, 0x00, 0x00, 0xff));
			draw_digits(draw_sprites, glm::vec2(411.0f, 365.0f), main_play->GetKeysCollected(), glm::u8vec4(0x00, 0x00, 0x00, 0xff));
			draw_digits(draw_sprites, glm::vec2(445.0f, 365.0f), main_play->GetTotalKeysToCollect(), glm::u8vec4(0x00, 0x00, 0x00, 0xff));
		}
	} //<-- gets drawn here!
	if (row_width == 4) {
		std::string text_string = "Atk: " + std::to_string(player->GetAttackPoint()) + "\nDef: " + std::to_string(player->GetDefense());
		if (items[selected].item_prototype) {
			text_string += "\n\nItem Description:\n" + items[selected].item_prototype->GetDescription();
		}
		text.SetText(text_string);
		text.Draw();
	}
	
	
	GL_ERRORS(); //PARANOIA: print errors just in case we did something wrong.
}

void MenuMode::grid_layout_items(glm::vec2 const& top_left, float horizontal_gap, float vertical_gap, int start_idx, int end_idx) {
	DrawSprites temp(*atlas, view_min, view_max, view_max - view_min, DrawSprites::AlignPixelPerfect); //<-- doesn't actually draw
	
	for (int i = start_idx; i < end_idx; i++) {
		Item* item = &items[i];
		item->transform.position_.x = (i - start_idx) % row_width * horizontal_gap + top_left.x;
		item->transform.position_.y = -(i - start_idx) / row_width * vertical_gap + top_left.y;
	}
}
void MenuMode::vertical_layout_items(float gap) {
	DrawSprites temp(*atlas, view_min, view_max, view_max - view_min, DrawSprites::AlignPixelPerfect); //<-- doesn't actually draw
	float y = view_max.y;
	for (auto& item : items) {
	glm::vec2 min { 0.0f }, max { 0.0f };
		if (item.sprite) {
			min = glm::vec2(item.transform.scale_.x * (item.sprite->min_px - item.sprite->anchor_px).x, item.transform.scale_.y * (item.sprite->min_px - item.sprite->anchor_px).y);
			max = glm::vec2(item.transform.scale_.x * (item.sprite->max_px - item.sprite->anchor_px).x, item.transform.scale_.y * (item.sprite->max_px - item.sprite->anchor_px).y);
		}
		item.transform.position_.y = y - max.y;
		item.transform.position_.x = 0.5f * (view_max.x + view_min.x) - 0.5f * (max.x + min.x);
		y = y - (max.y - min.y) - gap;
	}
	float ofs = -0.5f * y;
	for (auto& item : items) {
		item.transform.position_.y += ofs;
	}
}

void MenuMode::on_leave()
{
	Sound::play(*sound_samples["menu_exit"]);
}

void MenuMode::on_enter()
{
	Sound::play(*sound_samples["menu_enter"]);
}
