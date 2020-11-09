#pragma once

#include "GL.hpp"
#include "Load.hpp"

#include <glm/glm.hpp>

//Shader program that draws transformed, vertices tinted with vertex colors:
struct FontProgram {
	FontProgram();
	~FontProgram();

	GLuint program = 0;
	GLuint vertex_array = 0;
	//Attribute (per-vertex variable) locations:
	GLuint Position_vec4 = -1U;
	GLuint TexCoord_vec2 = -1U;
	//Uniform (per-invocation variable) locations:
	GLuint OBJECT_TO_CLIP_mat4 = -1U;
	GLuint Color_vec4 = -1U;
	//Textures:
	//TEXTURE0 - texture that is accessed by TexCoord

	struct Vertex {
		glm::vec2 Position;
		glm::vec2 TexCoord;
	};
	static_assert(sizeof(Vertex) == 2 * 4 + 2 * 4, "Vertex is packed.");

	struct Glyph {
		GLuint vertex_buffer { 0 };
		GLuint texture_id { 0 };

		Glyph(const glm::vec2& anchor, int width, int height, const void* pixels);
		Glyph(const Glyph&) = delete;
		Glyph(Glyph&&) = delete;
		~Glyph();
	};

	void DrawGlyphs(const std::vector<Glyph*>& glyphs, const glm::vec2& anchor, const glm::uvec2& drawable_size, const glm::vec4& color) const;
};

extern Load< FontProgram > font_program;
