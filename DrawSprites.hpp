#pragma once

/*
 * Helper class for drawing a bunch of sprites from a single atlas.
 * Usage:
 *	{
 *		DrawSprites draw_sprites(atlas, screen_min, screen_max, DrawSprites::PixelPerfect);
 *		draw_sprites.draw(
 *
 */

#include "Sprite.hpp"
#include "engine/Transform2D.hpp"

#include <glm/glm.hpp>

#include <vector>

struct DrawSprites {
	enum AlignMode {
		AlignPixelPerfect,
		AlignSloppy
	};
	//Starts accumulating sprites to draw on allocation:
	DrawSprites(SpriteAtlas const &atlas, glm::vec2 const &view_min, glm::vec2 const &view_max, glm::uvec2 const &drawable_size, AlignMode mode = AlignSloppy);

	//Add more sprites to draw:
	void draw(Sprite const &sprite, const Transform2D& transform, glm::u8vec4 const &tint = glm::u8vec4(0xff, 0xff, 0xff, 0xff));
	//draw a bounding box for debuging purpose:
	void draw_bounding_box(glm::vec2 lower_left_corner, glm::vec2 upper_right_corner);

	//Actually draws the sprites on deallocation:
	~DrawSprites();

	//--- internals ---
	SpriteAtlas const &atlas;
	glm::vec2 view_min, view_max;
	glm::uvec2 drawable_size;
	AlignMode mode;
	glm::mat4 to_clip;

	struct Vertex {
		Vertex(glm::vec2 const &Position_, glm::vec2 const &TexCoord_, glm::u8vec4 const &Color_) : Position(Position_), TexCoord(TexCoord_), Color(Color_) { }
		glm::vec2 Position;
		glm::vec2 TexCoord;
		glm::u8vec4 Color;
	};
	std::vector< Vertex > attribs;
};
