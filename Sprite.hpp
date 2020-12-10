#pragma once

/*
 * A sprite is a rectangular area in a atlas texture.
 * Sprites are loaded by creating a 'SpriteAtlas' (which both loads a texture
 * image and sprite position metadata); you can then look up individual
 * 'Sprite's in the 'SpriteAtlas' using its lookup() function.
 */

#include "GL.hpp"

#include <glm/glm.hpp>

#include <unordered_map>
#include <string>
#include <vector>

struct Sprite {
	//Sprites are rectangles in an atlas texture:
	glm::vec2 min_px; //position of lower left corner (in pixels; ll-origin)
	glm::vec2 max_px; //position of upper right corner (in pixels; ll-origin)
	glm::vec2 anchor_px; //position of 'anchor' (in pixels; ll-origin)
	glm::vec2 size_px;

	GLuint atlas_idx;

	//NOTE:
	//The 'anchor' is the "center" or "pivot point" of the sprite --
	// a value defined when authoring the sprite which is used as the
	// position of the sprite when drawing.

	//Generally, you set it to something meaningful like the center of mass
	// or the position of the feet.
};

struct SpriteAtlas {
	//load from filebase.png and filebase.atlas:
	SpriteAtlas() = default;
	~SpriteAtlas();

	//look up sprite in list of loaded sprites:
	// throws an error if name is missing
	Sprite const &lookup(std::string const &name) const;

	void LoadSprites(std::string const &filebase);
	size_t GetSpriteAtlasNum() const { return texes.size(); } 

	//this is the atlas texture; used when drawing sprites:
	std::vector<GLuint> texes {};
	// GLuint tex = 0;
	std::vector<glm::uvec2> tex_sizes {};

	//---- internal data ---

	//table of loaded sprites, sorted by name:
	std::unordered_map< std::string, Sprite > sprites {};

	//path to atlas, stored for debugging purposes:
	std::vector<std::string> atlas_pathes {};
};

