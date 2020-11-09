#include "FontProgram.hpp"

#include "gl_compile_program.hpp"
#include "gl_errors.hpp"

#include <glm/gtc/type_ptr.hpp>

Load<FontProgram> font_program(LoadTagEarly);

FontProgram::FontProgram()
{
	//Compile vertex and fragment shaders using the convenient 'gl_compile_program' helper function:
	program = gl_compile_program(
		//vertex shader:
		"#version 330\n"
		"uniform mat4 OBJECT_TO_CLIP;\n"
		"in vec4 Position;\n"
		"in vec2 TexCoord;\n"
		"out vec2 texCoord;\n"
		"void main() {\n"
		"	gl_Position = OBJECT_TO_CLIP * Position;\n"
		"	texCoord = TexCoord;\n"
		"}\n",
		//fragment shader:
		"#version 330\n"
		"uniform sampler2D TEX;\n"
		"uniform vec4 color;\n"
		"in vec2 texCoord;\n"
		"out vec4 fragColor;\n"
		"void main() {\n"
		"	fragColor = texture(TEX, texCoord) * color;\n"
		"}\n");
	//As you can see above, adjacent strings in C/C++ are concatenated.
	// this is very useful for writing long shader programs inline.

	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	glEnableVertexAttribArray(Position_vec4);
	glVertexAttribPointer(Position_vec4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, Position));

	glEnableVertexAttribArray(TexCoord_vec2);
	glVertexAttribPointer(TexCoord_vec2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, TexCoord));

	glBindVertexArray(0);

	//look up the locations of vertex attributes:
	Position_vec4 = glGetAttribLocation(program, "Position");
	TexCoord_vec2 = glGetAttribLocation(program, "TexCoord");

	//look up the locations of uniforms:
	OBJECT_TO_CLIP_mat4 = glGetUniformLocation(program, "OBJECT_TO_CLIP");
	Color_vec4 = glGetUniformLocation(program, "color");
	GLuint TEX_sampler2D = glGetUniformLocation(program, "TEX");

	//set TEX to always refer to texture binding zero:
	glUseProgram(program); //bind program -- glUniform* calls refer to this program now

	glUniform1i(TEX_sampler2D, 0); //set TEX to sample from GL_TEXTURE0

	glUseProgram(0); //unbind program -- glUniform* calls refer to ??? now
}

void FontProgram::DrawGlyphs(const std::vector<Glyph *> &glyphs, const glm::vec2 &anchor, const glm::uvec2 &drawable_size, const glm::vec4 &color) const
{
	glUseProgram(program);
	glUniform4fv(Color_vec4, 1, &color[0]);
	float aspect = static_cast<float>(drawable_size.x) / drawable_size.y;
	float scale = glm::min((2.0f * aspect) / drawable_size.x, 2.0f / drawable_size.y);
	glm::mat4 object_to_clip{
		glm::vec4(scale / aspect, 0.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, scale, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
		glm::vec4(anchor.x * (scale / aspect), anchor.y * scale, 0.0f, 1.0f)};
	glUniformMatrix4fv(OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(object_to_clip));
	glBindVertexArray(vertex_array);

	for (const auto &glyph : glyphs)
	{
		glBindBuffer(GL_ARRAY_BUFFER, glyph->vertex_buffer);

		glEnableVertexAttribArray(font_program->Position_vec4);
		glVertexAttribPointer(font_program->Position_vec4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, Position));

		glEnableVertexAttribArray(font_program->TexCoord_vec2);
		glVertexAttribPointer(font_program->TexCoord_vec2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, TexCoord));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, glyph->texture_id);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glBindVertexArray(0);
	glUseProgram(0);
	GL_ERRORS();
}

FontProgram::Glyph::Glyph(const glm::vec2 &anchor, int width, int height, const void *pixels)
{

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ONE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ONE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ONE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);

	glBindTexture(GL_TEXTURE_2D, 0);

	Vertex vertexes[]{
		{{anchor[0], anchor[1]}, {0, 0}},
		{{anchor[0] + width, anchor[1]}, {1, 0}},
		{{anchor[0], anchor[1] - height}, {0, 1}},
		{{anchor[0] + width, anchor[1] - height}, {1, 1}}};

	glGenBuffers(1, &vertex_buffer);
	glBindVertexArray(font_program->vertex_array);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), static_cast<const void *>(vertexes), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GL_ERRORS();
}

FontProgram::Glyph::~Glyph()
{
	if (vertex_buffer > 0)
	{
		glDeleteBuffers(1, &vertex_buffer);
		vertex_buffer = 0;
	}

	if (texture_id > 0)
	{
		glDeleteTextures(1, &texture_id);
		texture_id = 0;
	}
}

FontProgram::~FontProgram()
{
	glDeleteProgram(program);
	glDeleteVertexArrays(1, &vertex_array);
	vertex_array = 0;
	program = 0;
}
