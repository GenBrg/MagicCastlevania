#include "TextBase.hpp"

#include "../Load.hpp"
#include "../Util.hpp"
#include <stdexcept>
#include <iostream>

unsigned int index_buffer_content[] {0, 1, 2, 1, 2, 3};

GLuint TextBase::index_buffer_ {0};

Load<void> load_index_buffer(LoadTagEarly, [](){
	glGenBuffers(1, &TextBase::index_buffer_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TextBase::index_buffer_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), index_buffer_content, GL_STATIC_DRAW);
});


TextBase::TextBase() {

}


TextBase::TextBase(const std::string& font_path, int l_height): line_height_(l_height){
	FT_Library library;
	FT_Error error_lib = FT_Init_FreeType(&library);
	if (error_lib) {
		throw std::runtime_error("Init library failed.");
	}

	FT_Error error = FT_New_Face(library, &(font_path[0]), 0, &face_);
	if (error == FT_Err_Unknown_File_Format) {
		throw std::runtime_error("The font file could be opened and read, but it appears that its font format is unsupported!");
	} else if (error) {
		std::cout<<"error code: "<<(int)error<<std::endl;
		throw std::runtime_error("The font file could not be opened or read, or that it is broken!");
	}

	if (!face_) {
		throw std::runtime_error("Wrong font!");
	}
}


void TextBase::Draw(const glm::uvec2& window_size) {
	if(!buf_ || !font_) {
		return;
	}
	glUseProgram(texture2d_program->program);

	// in real pixel, top left is (0, 0)
	float cursor_x = (anchor_.x + 1.0f) * window_size.x / 2.0f;
	float cursor_y = (anchor_.y + 1.0f) * window_size.y / 2.0f;
	int line_cnt = 0;

	FT_GlyphSlot slot = face_->glyph;

	for (unsigned int i = 0; i < glyph_count_; ++i) {
		if(text_[i] == '\n') {
			line_cnt++;
			cursor_x = (anchor_.x + 1.0f) * window_size.x / 2.0f;
			cursor_y = (anchor_.y + 1.0f) * window_size.y / 2.0f - (float)(line_cnt * line_height_);
			continue;
		}

		auto glyphid = glyph_info_[i].codepoint;
		auto x_advance = glyph_pos_[i].x_advance / 64.0f;
		auto y_advance = glyph_pos_[i].y_advance / 64.0f;

		FT_Error error = FT_Load_Char(face_, text_[i], FT_LOAD_RENDER);
		if (error) {
			printf("%d\n", glyphid);
			continue;
		}

		auto& bitmap = slot->bitmap;

		float start_x = 2 * (cursor_x + slot->bitmap_left) / window_size.x - 1;
		float start_y = 2 * (cursor_y - bitmap.rows + slot->bitmap_top) / window_size.y - 1;
		float end_x = start_x + bitmap.width * 2.0f / window_size.x;
		float end_y = start_y + bitmap.rows * 2.0f / window_size.y;

		Texture2DProgram::Vertex vertexes[] {
			{{start_x, start_y}, color_, {0, 1}},
			{{end_x, start_y}, color_, {1, 1}},
			{{start_x, end_y}, color_, {0, 0}},
			{{end_x, end_y}, color_, {1, 0}}
		};

		GLuint texture_id = texture_ids_[i];
		GLuint vertex_buffer, vertex_array;

		glGenBuffers(1, &vertex_buffer);
		vertex_array = texture2d_program->GetVao(vertex_buffer);

		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Texture2DProgram::Vertex), static_cast<const void*>(vertexes), GL_STATIC_DRAW);

		glBindVertexArray(vertex_array);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, static_cast<const void*>(0));

		glDeleteBuffers(1, &vertex_buffer);
		glDeleteVertexArrays(1, &vertex_array);

		cursor_x += x_advance;
		cursor_y += y_advance;
	}
}

void TextBase::ClearText()
{
	for (GLuint texture_id : texture_ids_)
	{
		glDeleteTextures(1, &texture_id);
	}
	texture_ids_.clear();

	if (buf_) {
		hb_buffer_destroy(buf_);
		buf_ = nullptr;
	}

	if (font_) {
		hb_font_destroy(font_);
		font_ = nullptr;
	}
}

TextBase::~TextBase() {
	ClearText();
}

void TextBase::SetText(const std::string& text, FT_F26Dot6 size, glm::u8vec4 color, const glm::vec2& anchor) {
	ClearText();
	glm::vec2 transformed_anchor;
	transformed_anchor.x = anchor.x / INIT_WINDOW_W * 2.0f - 1.0f;
	transformed_anchor.y = anchor.y / INIT_WINDOW_H * 2.0f - 1.0f;
	anchor_ = transformed_anchor;
	color_ = color;
	text_ = text;

	FT_Set_Char_Size(face_, 0, size, 0, 0);
	font_ = hb_ft_font_create(face_, nullptr);

	buf_ = hb_buffer_create();
	hb_buffer_add_utf8(buf_, &text[0], -1, 0, -1);
	hb_buffer_set_direction(buf_, HB_DIRECTION_LTR);
	hb_buffer_set_script(buf_, HB_SCRIPT_LATIN);
	hb_buffer_set_language(buf_, hb_language_from_string("en", -1));
	hb_shape(font_, buf_, nullptr, 0);
	glyph_info_ = hb_buffer_get_glyph_infos(buf_, &glyph_count_);
	glyph_pos_ = hb_buffer_get_glyph_positions(buf_, &glyph_count_);

	FT_GlyphSlot slot = face_->glyph;

	for (unsigned int i = 0; i < glyph_count_; ++i) {

		FT_Error error = FT_Load_Char(face_, text[i], FT_LOAD_RENDER);
		if (error)
			continue;

		auto &bitmap = slot->bitmap;

		texture_ids_.emplace_back(0);

		GLuint &texture_id = texture_ids_.back();


		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ONE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ONE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ONE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap.width, bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
