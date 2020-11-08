#pragma once

#include "../Scene.hpp"
#include "../Texture2DProgram.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <hb.h>
#include <hb-ft.h>

#include <vector>

class TextBase {
private:
	FT_Face face_;

	std::vector<GLuint> texture_ids_;

	unsigned int glyph_count_;
	hb_font_t* font_ { nullptr };
	hb_buffer_t* buf_ { nullptr };
	hb_glyph_info_t* glyph_info_ { nullptr };
	hb_glyph_position_t* glyph_pos_ { nullptr };
	glm::vec2 anchor_;
	std::string text_;
	glm::u8vec4 color_;
	// in pixel
	int line_height_;

public:
	static GLuint index_buffer_;

	TextBase(const std::string& font_path, int l_height);
	TextBase();
	~TextBase();

	void Draw(const glm::uvec2& window_size);
	void SetText(const std::string& text, FT_F26Dot6 size, glm::u8vec4 color, const glm::vec2& anchor);
	void ClearText();
};
