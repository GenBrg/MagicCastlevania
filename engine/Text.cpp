#include "Text.hpp"

#include "../data_path.hpp"
#include "../Load.hpp"

#include <stdexcept>

FT_Library Text::library_;

Load<void> load_ft_library(LoadTagEarly, []() {
	FT_Error error;
	error = FT_Init_FreeType(&Text::library_);

	if (error)
	{
		throw std::runtime_error("FT_Init_FreeType error!");
	}
});

void Text::Draw(const glm::uvec2 &drawable_size)
{
	if (!visible_)
	{
		return;
	}

	font_program->DrawGlyphs(glyphs_, transform_.MakeLocalToWorld()[2], drawable_size, color_);
}

void Text::ClearText()
{
	line_number_ = 0;
	anchor_ = glm::vec2(0.0f, 0.0f);

	for (const auto& glyph : glyphs_) {
		delete glyph;
	}
	glyphs_.clear();
}

Text::~Text()
{
	ClearText();
}

Text& Text::SetText(const std::string& text)
{
	if (text == text_) {
		return *this;
	}

	if (text.length() > text_.length() && text_ == text.substr(0, text_.length())) {
		return AppendText(text.substr(text_.length()));
	}

	ClearText();
	return AppendText(text);
}

// pos: OpenGL position (-1, 1)
Text& Text::AppendText(const std::string& text)
{
	text_.append(text);
	FT_Set_Char_Size(face_, 0, height_, 0, 0);

	hb_font_t *font {nullptr};
	hb_buffer_t *buf {nullptr};
	font = hb_ft_font_create(face_, nullptr);
	buf = hb_buffer_create();
	hb_buffer_add_utf8(buf, text.c_str(), -1, 0, -1);
	hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
	hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
	hb_buffer_set_language(buf, hb_language_from_string("en", -1));
	hb_shape(font, buf, nullptr, 0);

	unsigned int glyph_count;
	hb_glyph_position_t *glyph_pos {nullptr};

	glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

	FT_GlyphSlot slot = face_->glyph;

	for (unsigned int i = 0; i < glyph_count; ++i)
	{
		auto new_line = [&]() {
			++line_number_;
			anchor_ = glm::vec2(0.0f, -line_number_ * height_ / 64.0f);
		};

		if (text[i] == '\n') {
			new_line();
			continue;
		}

		FT_Error error = FT_Load_Char(face_, text[i], FT_LOAD_RENDER);
		if (error)
			continue;

		auto x_offset = glyph_pos[i].x_offset / 64.0f;
		auto y_offset = glyph_pos[i].y_offset / 64.0f;
		auto x_advance = glyph_pos[i].x_advance / 64.0f;
		auto y_advance = glyph_pos[i].y_advance / 64.0f;

		if (line_width_ > 0 && x_offset + anchor_.x > line_width_) {
			new_line();
		}

		auto &bitmap = slot->bitmap;
		glyphs_.push_back(new FontProgram::Glyph(
			{anchor_.x + x_offset + slot->bitmap_left, anchor_.y + y_offset + slot->bitmap_top},
			 bitmap.width, bitmap.rows, bitmap.buffer));

		anchor_ += glm::vec2(x_advance, y_advance);
	}

	hb_buffer_destroy(buf);
	hb_font_destroy(font);

	return *this;
}

Text& Text::SetPos(const glm::vec2 &text_start_position)
{
	transform_.position_ = text_start_position;
	return *this;
}

Text& Text::SetLineWidth(int line_width)
{
	line_width_ = line_width;
	return *this;
}

Text& Text::SetFontSize(FT_F26Dot6 height)
{
	height_ = height;
	return *this;
}

Text& Text::SetVisibility(bool visible)
{
	visible_ = visible;
	return *this;
}

Text& Text::SetColor(const glm::u8vec4 &color)
{
	color_ = color;
	color_ /= 255.0f;
	return *this;
}

Text::Text(const std::string& font_path, Transform2D* parent_transform) :
transform_(parent_transform)
{
	auto it = cached_faces_.find(font_path);
	if (it != cached_faces_.end()) {
		face_ = (*it).second;
		return;
	}

	if (FT_New_Face(library_, font_path.c_str(), 0, &face_))
	{
		throw std::runtime_error("ERROR::FREETYPE: Failed to load font " + font_path);
	}

	cached_faces_[font_path] = face_;
}
