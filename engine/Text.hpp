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

public:
	static FT_Library library_;

	Text(const Text &) = delete;

	/** Construct a new text.
	 *  @param font_path Path to the font file used to define the font of the text.
	 *  @param parent_transform Parent transform the font is attached to, pass nullptr if the text is in world space.
	 */
	Text(const std::string& font_path, Transform2D* parent_transform);
	~Text();

	/** Sets the text content. It will remove the old glyph vertex and texture data in the GPU if it exists
	 *  and upload new vertices and texture data to GPU for rendering.
	 * @param text Text Content.
	 * @param height The height of a single glyph, the unit is 1/64 pixel.
	 * @param line_width The maximum width of a single line, the unit is pixel.
	 */
	Text& SetText(const char *text, FT_F26Dot6 height, int line_width);

	/** Sets the color of the text
	 */
	Text& SetColor(const glm::u8vec4 &color);

	/** Sets the position of the font.
	 *  @param anchor The upper left corner of the text, the unit is pixel.
	 */
	Text& SetPos(const glm::vec2 &anchor);

	/** Sets the position of the font.
	 *  @param visible If the text is visible.
	 */
	Text& SetVisibility(bool visible);

	/** Clears all the glyphs in the text.
	 */
	void ClearText();

	/** Draw text to screen.
	 *  @param drawable_size The window drawable size.
	 */
	void Draw(const glm::uvec2 &drawable_size);
};
