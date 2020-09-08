#include "PPU466.hpp"

#include "GL.hpp"
#include <Load.hpp>

#include <vector>

//In order to implement the PPU466 on modern graphics hardware, a fancy, special purpose tile-drawing shader is used:
struct PPUTileProgram {
	PPUTileProgram();
	~PPUTileProgram();

	GLuint program = 0;

	//Attribute (per-vertex variable) locations:
	GLuint Position_vec2 = -1U;
	GLuint TileCoord_ivec2 = -1U;
	GLuint Palette_int = -1U;

	//Uniform (per-invocation variable) locations:
	GLuint OBJECT_TO_CLIP_mat4 = -1U;

	//Textures bindings:
	//TEXTURE0 - the tile table (as a 128x128 R8UI texture)
	//TEXTURE1 - the palette table (as a 4x8 RGBA8 texture)
};

//Initialize tile program and associated buffers:
Load< PPUTileProgram > tile_program(LoadTagInit); //will just 'new PPUTileProgram()' by default

//PPU data is streamed to the GPU (read: uploaded 'just in time') using a few buffers:
struct PPUDataStream {
	PPUDataStream();
	~PPUDataStream();

	//vertex format for convenience:
	struct Vertex {
		Vertex(glm::ivec2 const &Position_, glm::ivec2 const &TileCoord_, int32_t const &Palette_)
			: Position(Position_), TileCoord(TileCoord_), Palette(Palette_) { }
		//I generally make class members lowercase, but I make an exception here because
		// I use uppercase for vertex attributes in shader programs and want to match.
		glm::ivec2 Position;
		glm::ivec2 TileCoord;
		int32_t Palette;
	};

	//vertex buffer that will store data stream:
	GLuint vertex_buffer = 0;

	//vertex array object that maps tile program attributes to vertex storage:
	GLuint vertex_buffer_for_tile_program = 0;

	//texture object that will store tile table:
	GLuint tile_tex = 0;

	//texture object that will store palette table:
	GLuint palette_tex = 0;
};

Load< PPUDataStream > data_stream(LoadTagDefault);

//-------------------------------------------------------------------

PPU466::PPU466() {
	for (auto &palette : palette_table) {
		palette[0] = glm::u8vec4(0x00, 0x00, 0x00, 0x00);
		palette[1] = glm::u8vec4(0x44, 0x44, 0x44, 0xff);
		palette[2] = glm::u8vec4(0x99, 0x99, 0x99, 0xff);
		palette[3] = glm::u8vec4(0xff, 0xff, 0xff, 0xff);
	}

	for (auto &tile : tile_table) {
		tile.bit0.set({ 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0 });
		tile.bit1.set({ 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff });
	}

	background.assign(0);
}

void PPU466::draw(glm::uvec2 const &drawable_size) const {
	//this code does screen scaling by manipulating the viewport, so save old values:
	GLuint old_viewport[4];
	glGetIntegerv(GL_VIEWPORT, old_viewport);

	//draw to whole drawable:
	glViewport(0,0,drawable_size.x,drawable_size.y);

	//background gets background color:
	glClearColor(
		background_color.r / 255.0f, 
		background_color.g / 255.0f, 
		background_color.b / 255.0f, 
		background_color.a / 255.0f
	);
	glClear(GL_COLOR_BUFFER_BIT);

	//set up screen scaling:
	if (drawable_size.x < ScreenWidth || drawable_size.y < ScreenHeight) {
		//if screen is too small, just do some inglorious pixel-mushing:
		//(viewport is already set. nothing more to do.)
	} else {
		//otherwise, do careful integer-multiple upscaling:
		//largest size that will fit in the drawable:
		const uint32_t scale = std::max( 1, std::min(drawable_size.x / ScreenWidth, drawable_size.y / ScreenHeight) );

		//compute lower left so that screen is centered:
		const glm::ivec2 lower_left = glm::ivec2(
			(int32_t(drawable_size.x) - scale * int32_t(ScreenWidth)) / 2,
			(int32_t(drawable_size.y) - scale * int32_t(ScreenHeight)) / 2
		);
		glViewport(lower_left.x, lower_left.y, scale * ScreenWidth, scale * ScreenHeight);
	}

	//build triangle strip representing background and sprites:

	constexpr uint32_t TristripSize = 6 * (BackgroundWidth * BackgroundHeight + sprite_table.size());
	std::vector< PPUStream::Vertex > triangle_strip;
	triangle_strip.reserve(TristripSize);

	//helper to put a single tile somewhere on the screen:
	auto draw_tile = [&triangle_strip](glm::ivec2 const &lower_left, uint8_t tile_index, uint8_t palette_index){
		//convert tile index to lower-left pixel coordinate in tile image:
		glm::ivec2 tile_coord = glm::ivec2((tile_index % 16)*8, (tile_index / 16)*8);

		//build a quad as a (very short) triangle strip that starts and ends with degenerate triangles:
		triangle_strip.emplace_back(glm::ivec2(lower_left.x+0, lower_left.y+0), glm::ivec2(tile_coord.x+0, tile_coord.y+0), palette_index);
		triangle_strip.emplace_back(triangle_strip.back());
		triangle_strip.emplace_back(glm::ivec2(lower_left.x+0, lower_left.y+8), glm::ivec2(tile_coord.x+0, tile_coord.y+8), palette_index);
		triangle_strip.emplace_back(glm::ivec2(lower_left.x+8, lower_left.y+0), glm::ivec2(tile_coord.x+8, tile_coord.y+0), palette_index);
		triangle_strip.emplace_back(glm::ivec2(lower_left.x+8, lower_left.y+8), glm::ivec2(tile_coord.x+8, tile_coord.y+8), palette_index);
		triangle_strip.emplace_back(triangle_strip.back());
	};

	//helper to draw the sprite list (used because we need to draw the 'behind' sprites, then the background, then the 'front' sprites:
	auto draw_sprites = [this,&draw_tile](uint8_t priority) {
		for (auto const &sprite : sprites) {
			if (sprite.attributes & 0x80 != priority) continue;
			draw_tile(
				glm::ivec2(sprite.x, sprite.y),
				sprite.index,
				sprite.attributes & 0x07 //just the palette index part
			);
		}
	};

	draw_sprites(0x80); //draw sprites with priority == 1 ('behind' sprites)

	{ //draw the background:
		//To simulate the 'infinite tiling' behavior this code draws the background as four screen-sized chunks,
		// each of which is drawn at an offset that causes it to overlap the screen.

		static_assert(BackgroundWidth * 8 == ScreenWidth * 2, "Background should be exactly twice the screen width.");
		static_assert(BackgroundHeight * 8 == ScreenHeight * 2, "Background should be exactly twice the screen height.");

		for (int32_t chunk_y : {0, ScreenHeight}) {
			for (int32_t chunk_x : {0, ScreenWidth}) {
				//position of the lower-left corner of the chunk:
				glm::ivec2 pos = glm::ivec2(chunk_x, chunk_y) + background_position;

				constexpr int32_t BackgroundWidthPixels = int32_t(BackgroundWidth) * 8;
				constexpr int32_t BackgroundHeightPixels = int32_t(BackgroundHeight) * 8;

				//reduce to (-BackgroundWidthPixels,0] x (-BackgroundHeightPixels,0]:
				pos.x = ((pos.x % BackgroundWidthPixels) - BackgroundWidthPixels) % BackgroundWidthPixels;
				pos.y = ((pos.x % BackgroundHeightPixels) - BackgroundHeightPixels) % BackgroundHeightPixels;

				//move chunk if it doesn't overlap the screen:
				if (pos.x + int32_t(ScreenWidth) <= 0) pos.x += ScreenWidth;
				if (pos.y + int32_t(ScreenHeight) <= 0) pos.x += ScreenHeight;

				int32_t ox = chunk_x / 8;
				int32_t oy = chunk_y / 8;
				for (int32_t y = 0; y < BackgroundHeight/2; ++y) {
					for (int32_t x = 0; x < BackgroundWidth/2; ++x) {
						uint16_t info = background[(x + ox) + BackgroundWidth * (y + oy)];
						draw_tile(
							glm::ivec2(pos.x + 8*x, pos.y + 8*y),
							info & 0xff, //extract tile index bits
							(info) & 0x07 //extract palette index bits
						);
					}
				}

			}
		}

		for (uint32_t y = 0; y < BackgroundHeight; ++y) {
			for (uint32_t x = 0; x < BackgroundWidth; ++x) {
			}
		}
	}

	draw_sprites(0x00); //draw sprites with priority == 0 ('in front' sprites)
}




