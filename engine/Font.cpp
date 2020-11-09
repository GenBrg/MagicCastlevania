#include "Font.hpp"
#include "data_path.hpp"

#include "Load.hpp"

#include <stdexcept>
#include <iostream>

unsigned int index_buffer_content[]{0, 1, 2, 1, 2, 3};

GLuint Font::index_buffer_{0};
FT_Library Font::library_;

Load<void> load_index_buffer(LoadTagEarly, []() {
	glGenBuffers(1, &Font::index_buffer_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Font::index_buffer_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), index_buffer_content, GL_STATIC_DRAW);
});

Load<void> load_ft_library(LoadTagEarly, []() {
	FT_Error error;
	error = FT_Init_FreeType(&Font::ft_);

	if (error)
	{
		throw std::runtime_error("FT_Init_FreeType error!");
	}
});

void Font::Draw(const glm::uvec2 &drawable_size)
{
	if (!visible_)
	{
		return;
	}

	glUseProgram(texture2d_program->program);

	float cursor_x = GetPixelPos(anchor_.x, drawable_size.x), cursor_y = GetPixelPos(anchor_.y, drawable_size.y);
	FT_GlyphSlot slot = face_->glyph;

	for (unsigned int i = 0; i < glyph_count_; ++i)
	{
		auto x_offset = glyph_pos_[i].x_offset / 64.0f;
		auto y_offset = glyph_pos_[i].y_offset / 64.0f;
		auto x_advance = glyph_pos_[i].x_advance / 64.0f;
		auto y_advance = glyph_pos_[i].y_advance / 64.0f;

		FT_Error error = FT_Load_Char(face_, text_[i], FT_LOAD_RENDER);
		if (error)
			continue;

		auto &bitmap = slot->bitmap;

		float start_x = GetOpenGLPos(cursor_x + x_offset + slot->bitmap_left, drawable_size.x);
		float start_y = GetOpenGLPos(cursor_y + y_offset + slot->bitmap_top, drawable_size.y);
		float end_x = start_x + bitmap.width * 2.0f / drawable_size.x;
		float end_y = start_y - bitmap.rows * 2.0f / drawable_size.y;

		Texture2DProgram::Vertex vertexes[]{
			{{start_x, start_y}, color_, {0, 0}},
			{{end_x, start_y}, color_, {1, 0}},
			{{start_x, end_y}, color_, {0, 1}},
			{{end_x, end_y}, color_, {1, 1}}};

		GLuint texture_id = texture_ids_[i];
		GLuint vertex_buffer, vertex_array;

		glGenBuffers(1, &vertex_buffer);
		vertex_array = texture2d_program->GetVao(vertex_buffer);

		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Texture2DProgram::Vertex), static_cast<const void *>(vertexes), GL_STATIC_DRAW);

		glBindVertexArray(vertex_array);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, static_cast<const void *>(0));

		glDeleteBuffers(1, &vertex_buffer);
		glDeleteVertexArrays(1, &vertex_array);

		cursor_x += x_advance;
		cursor_y += y_advance;
	}
}

void Font::ClearText()
{
	for (GLuint texture_id : texture_ids_)
	{
		glDeleteTextures(1, &texture_id);
	}
	texture_ids_.clear();

	if (buf_)
	{
		hb_buffer_destroy(buf_);
		buf_ = nullptr;
	}

	if (font_)
	{
		hb_font_destroy(font_);
		font_ = nullptr;
	}
}

Font::~Font()
{
	ClearText();
}

// pos: OpenGL position (-1, 1)
Font& Font::SetText(const char *text, FT_F26Dot6 size)
{
	ClearText();

	text_ = text;

	FT_Set_Char_Size(face_, 0, size, 0, 0);
	font_ = hb_ft_font_create(face_, nullptr);

	buf_ = hb_buffer_create();
	hb_buffer_add_utf8(buf_, text, -1, 0, -1);
	hb_buffer_set_direction(buf_, HB_DIRECTION_LTR);
	hb_buffer_set_script(buf_, HB_SCRIPT_LATIN);
	hb_buffer_set_language(buf_, hb_language_from_string("en", -1));
	hb_shape(font_, buf_, nullptr, 0);
	glyph_info_ = hb_buffer_get_glyph_infos(buf_, &glyph_count_);
	glyph_pos_ = hb_buffer_get_glyph_positions(buf_, &glyph_count_);

	FT_GlyphSlot slot = face_->glyph;

	for (unsigned int i = 0; i < glyph_count_; ++i)
	{

		FT_Error error = FT_Load_Char(face_, text[i], FT_LOAD_RENDER);
		if (error)
			continue;

		auto &bitmap = slot->bitmap;

		texture_ids_.emplace_back(0);

		GLuint &texture_id = texture_ids_.back();

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap.width, bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ONE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ONE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ONE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);

		glBindTexture(GL_TEXTURE_2D, 0);

		// while (GLenum error = glGetError())
		// {
		//     std::cout << "[OpenGL Error] (" << error << "):" << std::endl;
		// }
		return *this;
	}
}

Font& Font::SetPos(const glm::vec2 &anchor)
{
	anchor_ = anchor;
	return *this;
}

Font& Font::SetVisibility(bool visible)
{
	visible_ = visible;
	return *this;
}

Font& Font::SetColor(const glm::u8vec4 &color)
{
	color_ = color;
	return *this;
}
