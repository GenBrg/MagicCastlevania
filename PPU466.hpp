#pragma once

/*
 * PPU466 -- a very limited graphics system [loosely] based on the NES's PPU.
 * 
 *
 */

#include <glm/glm.hpp>
#include <array>

struct PPU466 {
	PPU466();

	//--------------------------------------------------------------
	//Call these functions to draw with the PPU:

	//when you wish the PPU to draw, tell it so:
	// pass the size of the current framebuffer in pixels so it knows how to scale itself
	void draw(glm::uvec2 const &drawable_size) const;

	//for debugging, you can ask the PPU to draw its current tiles, palettes, etc:
	// pass the size of the current framebuffer in pixels so it knows how to scale itself
	//someday, maybe: void draw_DEBUG_overlay(glm::uvec2 drawable_size) const;

	//--------------------------------------------------------------
	//Set the values below to control the PPU's drawing:

	//The PPU's screen is 256x240:
	// the origin -- pixel (0,0) -- is in the lower left
	enum : uint32_t {
		ScreenWidth = 256,
		ScreenHeight = 240
	};

	//Background Color:
	// The PPU clears the screen to the background color before other drawing takes place:
	// the screen is cleared to this color before any other drawing takes place
	glm::u8vec3 background_color = glm::u8vec3(0x00, 0x00, 0x00);

	//Palette:
	// The PPU uses 4-bit indexed color.
	// thus, a color palette has four entries:
	typedef std::array< glm::u8vec4, 4 > Palette;
	// for a "true NES" experience, you should set:
	//   color 0 to fully transparent
	//   and color 1-3 to fully opaque.

	//Palette Table:
	// The PPU stores 8 palettes for use when drawing tiles:
	std::array< Palette, 8 > palette_table;

	//Tile:
	// The PPU uses 8x8 4-bit indexed-color tiles:
	// each tile is stored as two 8x8 "bit plane" images
	//   each bit-plane image is stored in rows from bottom-to-top
	//   each bit in a row corresponds to a pixel in increasing order:
	//      [ b0 b1 b2 b3 b4 b5 b6 b7 ]
	//
	// For eaxmple, to read the color index at pixel (2,7):
	//  bit0_at_2_7 = (tile.bit0[7] >> 2) & 1;
	//  bit1_at_2_7 = (tile.bit1[7] >> 2) & 1;
	//  color_index_at_2_7 = (bit1_at_2_7 << 1) | bit0_at_2_7;
	struct Tile {
		std::array< uint8_t, 8 > bit0; //<-- controls bit 0 of the color index
		std::array< uint8_t, 8 > bit1; //<-- controls bit 1 of the color index
	};
	static_assert(sizeof(Tile) == 16, "Tile is packed");

	//Tile Table:
	// The PPU has a 256-tile 'pattern memory' in which tiles are stored:
	//  this is often thought of as a 16x16 grid of tiles.
	std::array< Tile, 16 * 16 > tile_table;

	//Background Layer:
	// The PPU's background layer is made of 64x60 tiles (512 x 480 pixels):
	enum : uint32_t {
		BackgroundWidth = 64,
		BackgroundHeight = 60
	};

	// The background is stored as a row-major grid of 16-bit values:
	//  the origin of the grid (tile (0,0)) is the bottom left of the grid
	//  each value in the grid gives:
	//    - bits 0-7: tile table index
	//    - bits 8-10: palette table index
	//    - bits 11-15: unused, should be 0
	//
	//  bits:  F E D C B A 9 8 7 6 5 4 3 2 1 0
	//        |---------|-----|---------------|
	//            ^        ^        ^-- tile index
	//            |        '----------- palette index
	//            '-------------------- unused (set to zero)
	std::array< uint16_t, BackgroundWidth * BackgroundHeight > background;

	//Background Position:
	// The background's lower-left pixel can positioned anywhere
	//   this can be used to "scroll the screen".
	glm::ivec2 background_position = glm::ivec2(0,0);
	//
	// screen pixels "outside the background" wrap around to the other side.
	// thus, background_position values of (x,y) and of (x+n*512,y+m*480) for
	// any integers n,m will look the same

	//Sprite:
	// On the PPU, all non-background objects are called 'sprites':
	//
	//  sprite positions (x,y) place the bottom-left of the sprite...
	//      ... x pixels from the left of the screen
	//      ... y pixels from the bottom of the screen
	//
	//  the sprite index is an index into the tile table
	//
	//  the sprite 'attributes' byte gives:
	//   bits:  7 6 5 4 3 2 1 0
	//         |-|-------|-----|
	//          ^    ^      ^
	//          |    |      '---- palette index (bits 0-2)
	//          |    '----------- unused (set to zero)
	//          '---------------- priority bit (bit 7)
	//
	//  the 'priority bit' chooses whether to render the sprite
	//   in front of (priority = 0) the background
	//   or behind (priority = 1) the background
	//
	struct Sprite {
		uint8_t x = 0; //x position. 0 is the left edge of the screen.
		uint8_t y = 240; //y position. 0 is the bottom edge of the screen. >= 240 is off-screen
		uint8_t index = 0; //index into tile table
		uint8_t attributes = 0; //tile attribute bits
	};
	static_assert(sizeof(Sprite) == 4, "Sprite is a 32-bit value.");
	//
	// The observant among you will notice that you can't draw a sprite moving off the left
	//  or bottom edges of the screen. Yep! This is [similar to] a limitation of the NES PPU!


	//Sprites:
	// The PPU always draws exactly 64 sprites:
	//  any sprites you don't want to use should be moved off the screen (y >= 240)
	std::array< Sprite, 64 > sprites;

};
