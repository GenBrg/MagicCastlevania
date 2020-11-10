#pragma once

#include "../FontProgram.hpp"
#include "Transform2D.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <hb.h>
#include <hb-ft.h>

#include <vector>
#include <unordered_map>
#include <string>

/** Class for rendering text.
 *  One text represents a paragraph which has many glyphs and may contain multiple lines.
 */
class Text
{
private:
	inline static std::unordered_map<std::string, FT_Face> cached_faces_;

	FT_Face face_;
	std::vector<FontProgram::Glyph*> glyphs_;
	Transform2D transform_;
	std::string text_;
	glm::vec4 color_ {1.0f, 1.0f, 1.0f, 1.0f};
	bool visible_ {true};

	int line_number_ { 0 };
	glm::vec2 anchor_ { 0.0f };
	int line_width_ { 0 };
	FT_F26Dot6 height_ { 2000 };

public:
	static FT_Library library_;

	Text(const Text &) = delete;

	/** Construct a new text.
	 *  @param font_path Path to the font file used to define the font of the text.
	 *  @param parent_transform Parent transform the font is attached to, pass nullptr if the text is in world space.
	 */
	Text(const std::string& font_path, Transform2D* parent_transform);
	~Text();

	/** Append glyphs in the string to the current glyph buffer. It will upload new vertices and texture data to GPU for rendering.
	 * @param text Text Content.
	 */
	Text& AppendText(const std::string& text);

	/** Set the content of the text.
	 *  Optimization has been done here where if the new text is the same as the previous text, nothing will happen.
	 *  If the new text is appending some characters, it will append the corresponding characters to the text.
	 *  Otherwise, it will clean the previous text and set the new text.
	 * @param text Text Content.
	 */
	Text& SetText(const std::string& text);

	/** Sets the maximum width of the line.
	 * @param line_width The maximum width of a single line, the unit is pixel, pass 0 or negative number to cancel the limitation.
	 */
	Text& SetLineWidth(int line_width);

	/** Sets the size of the font.
	 *  @param height The height of a single glyph, the unit is 1/64 pixel.
	 */
	Text& SetFontSize(FT_F26Dot6 height);

	/** Sets the color of the text
	 */
	Text& SetColor(const glm::u8vec4 &color);

	/** Sets the start position of the text.
	 *  @param text_start_position The upper left corner of the text, the unit is pixel.
	 * 					(0, 0) is the lower left corner of the window. x increases from left to right
	 * 				 and y increases from bottom to up.
	 */
	Text& SetPos(const glm::vec2 &text_start_position);

	/** Sets the position of the font.
	 *  @param visible If the text is visible.
	 */
	Text& SetVisibility(bool visible);

	/** Clears all the glyphs in the text, removes all the vertices and textures in the GPU.
	 */
	void ClearText();

	/** Draw text to screen.
	 *  @param drawable_size The window drawable size.
	 */
	void Draw(const glm::uvec2 &drawable_size);
};
