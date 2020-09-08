#include "PPU466.hpp"

#include "Load.hpp"
#include "GL.hpp"
#include "gl_compile_program.hpp"
#include "gl_errors.hpp"

#include <glm/gtc/type_ptr.hpp>

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
Load< PPUTileProgram > tile_program(LoadTagEarly); //will 'new PPUTileProgram()' by default

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
		tile.bit0 = { 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0 };
		tile.bit1 = { 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff };
	}

	for (uint32_t i = 0; i < background.size(); ++i) {
		background[i] = int16_t(
			  (i % 8) << 8 //cycle through all palettes
			| (i % palette_table.size()) //cycle through all tiles
		);
	}
}

void PPU466::draw(glm::uvec2 const &drawable_size) const {
	//this code does screen scaling by manipulating the viewport, so save old values:
	GLint old_viewport[4];
	glGetIntegerv(GL_VIEWPORT, old_viewport);

	//draw to whole drawable:
	glViewport(0,0,drawable_size.x,drawable_size.y);

	//background gets background color:
	glClearColor(
		background_color.r / 255.0f, 
		background_color.g / 255.0f, 
		background_color.b / 255.0f,
		1.0f
	);
	glClear(GL_COLOR_BUFFER_BIT);

	//set up screen scaling:
	if (drawable_size.x < ScreenWidth || drawable_size.y < ScreenHeight) {
		//if screen is too small, just do some inglorious pixel-mushing:
		//(viewport is already set. nothing more to do.)
	} else {
		//otherwise, do careful integer-multiple upscaling:
		//largest size that will fit in the drawable:
		const uint32_t scale = std::max( 1U, std::min(drawable_size.x / ScreenWidth, drawable_size.y / ScreenHeight) );

		//compute lower left so that screen is centered:
		const glm::ivec2 lower_left = glm::ivec2(
			(int32_t(drawable_size.x) - scale * int32_t(ScreenWidth)) / 2,
			(int32_t(drawable_size.y) - scale * int32_t(ScreenHeight)) / 2
		);
		glViewport(lower_left.x, lower_left.y, scale * ScreenWidth, scale * ScreenHeight);
	}

	//build triangle strip representing background and sprites:

	constexpr uint32_t TristripSize = uint32_t(6 * (BackgroundWidth * BackgroundHeight + decltype(sprites)().size()));
	std::vector< PPUDataStream::Vertex > triangle_strip;
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
			if ((sprite.attributes & 0x80) != priority) continue;
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

		for (int32_t chunk_y : {0, int32_t(ScreenHeight)}) {
			for (int32_t chunk_x : {0, int32_t(ScreenWidth)}) {
				//position of the lower-left corner of the chunk:
				glm::ivec2 pos = glm::ivec2(chunk_x, chunk_y) + background_position;

				constexpr int32_t BackgroundWidthPixels = int32_t(BackgroundWidth) * 8;
				constexpr int32_t BackgroundHeightPixels = int32_t(BackgroundHeight) * 8;

				//reduce to (-BackgroundWidthPixels,0] x (-BackgroundHeightPixels,0]:
				pos.x = ((pos.x % BackgroundWidthPixels) - BackgroundWidthPixels) % BackgroundWidthPixels;
				pos.y = ((pos.y % BackgroundHeightPixels) - BackgroundHeightPixels) % BackgroundHeightPixels;

				//move chunk if it doesn't overlap the screen:
				if (pos.x + int32_t(ScreenWidth) <= 0) pos.x += BackgroundWidthPixels;
				if (pos.y + int32_t(ScreenHeight) <= 0) pos.y += BackgroundHeightPixels;

				int32_t ox = chunk_x / 8;
				int32_t oy = chunk_y / 8;
				for (int32_t y = 0; y < int32_t(BackgroundHeight)/2; ++y) {
					for (int32_t x = 0; x < int32_t(BackgroundWidth)/2; ++x) {
						uint16_t info = background[(x + ox) + BackgroundWidth * (y + oy)];
						draw_tile(
							glm::ivec2(pos.x + 8*x, pos.y + 8*y),
							info & 0xff, //extract tile index bits
							(info >> 8) & 0x07 //extract palette index bits
						);
					}
				}

			}
		}
	}

	draw_sprites(0x00); //draw sprites with priority == 0 ('in front' sprites)

	assert(triangle_strip.size() == TristripSize && "Triangle strip size was estimated exactly.");

	//-------------------------------------------------
	//Upload at to GPU using PPUDataStream:

	{ //upload palette texture:
		static_assert(sizeof(palette_table) == 4 * 4 * decltype(palette_table)().size(), "palette table is packed");
		glBindTexture(GL_TEXTURE_2D, data_stream->palette_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 4, GLsizei(palette_table.size()), 0, GL_RGBA, GL_UNSIGNED_BYTE, palette_table.data());
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	{ //build + upload tile table texture:
		//interpret tiles and build a 128 x 128 index texture:
		static std::array< uint8_t, 128 * 128 > data;
		for (uint32_t i = 0; i < tile_table.size(); ++i) {
			Tile const &tile = tile_table[i];

			//location of tile in the texture:
			uint32_t ox = (i % 16) * 8;
			uint32_t oy = (i / 16) * 8;

			//copy tile indices into texture:
			for (uint32_t y = 0; y < 8; ++y) {
				for (uint32_t x = 0; x < 8; ++x) {
					data[ox+x + 128 * (oy+y)] =
						  ((tile.bit0[y] >> x) & 1)
						| ((tile.bit1[y] >> x) & 1) << 1;
				}
			}
		}

		glBindTexture(GL_TEXTURE_2D, data_stream->tile_tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, 128, 128, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, data.data());
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	{ //upload vertex data:
		glBindBuffer(GL_ARRAY_BUFFER, data_stream->vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(decltype(triangle_strip[0])) * triangle_strip.size(), triangle_strip.data(), GL_STREAM_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	//set up the pipeline:
	// set blending function for output fragments:
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// set the shader programs:
	glUseProgram(tile_program->program);

	// configure attribute streams:
	glBindVertexArray(data_stream->vertex_buffer_for_tile_program);

	// set uniforms for shader programs:
	{ //set matrix to transform [0,ScreenWidth]x[0,ScreenHeight] -> [-1,1]x[-1,1]:
		//NOTE: glm uses column-major matrices:
		glm::mat4 OBJECT_TO_CLIP = glm::mat4(
			glm::vec4(2.0f / ScreenWidth, 0.0f, 0.0f, 0.0f),
			glm::vec4(0.0f, 2.0f / ScreenHeight, 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
			glm::vec4(-1.0f,-1.0f, 0.0f, 1.0f)
		);
		glUniformMatrix4fv(tile_program->OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(OBJECT_TO_CLIP));
	}

	// bind texture units to proper texture objects:
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, data_stream->palette_tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, data_stream->tile_tex);

	//now that the pipeline is configured, trigger drawing of triangle strip:
	glDrawArrays(GL_TRIANGLE_STRIP, 0, GLsizei(triangle_strip.size()));

	//return state to default:
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);
	glUseProgram(0);

	glDisable(GL_BLEND);

	//also restore viewport, since earlier scaling code messed with it:
	glViewport(old_viewport[0], old_viewport[1], old_viewport[2], old_viewport[3]);

	GL_ERRORS();
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

PPUTileProgram::PPUTileProgram() {
	program = gl_compile_program(
		//vertex shader:
		"#version 330\n"
		"uniform mat4 OBJECT_TO_CLIP;\n"
		"in vec4 Position;\n"
		"in ivec2 TileCoord;\n"
		"in int Palette;\n"
		"out vec2 tileCoord;\n"
		"flat out int palette;\n"
		"void main() {\n"
		"	gl_Position = OBJECT_TO_CLIP * Position;\n"
		"	tileCoord = TileCoord;\n"
		"	palette = Palette;\n"
		"}\n"
	,
		//fragment shader:
		"#version 330\n"
		"uniform usampler2D TILE_TABLE;\n"
		"uniform sampler2D PALETTE_TABLE;\n"
		"in vec2 tileCoord;\n"
		"flat in int palette;\n" //"flat" means "uses the value of the provoking [by default, last] vertex in the primitive"
		"out vec4 fragColor;\n"
		"void main() {\n"
		"	uint index = texelFetch(TILE_TABLE, ivec2(tileCoord), 0).r;\n"
		"	fragColor = texelFetch(PALETTE_TABLE, ivec2(index, palette), 0);\n"
		//"	fragColor = vec4(float(index)/4.0,float(palette)/8,1,1);\n"
		//"	fragColor = texelFetch(TILE_TABLE, ivec2(int(gl_FragCoord.x) % textureSize(TILE_TABLE,0).x, int(gl_FragCoord.y) % textureSize(TILE_TABLE,0).y), 0);\n"
		//"	fragColor = texelFetch(PALETTE_TABLE, ivec2(int(gl_FragCoord.x) % textureSize(PALETTE_TABLE,0).x, int(gl_FragCoord.y) % textureSize(PALETTE_TABLE,0).y), 0);\n"
		"}\n"
	);

	//look up the locations of vertex attributes:
	Position_vec2 = glGetAttribLocation(program, "Position");
	TileCoord_ivec2 = glGetAttribLocation(program, "TileCoord");
	Palette_int = glGetAttribLocation(program, "Palette");

	//look up the locations of uniforms:
	OBJECT_TO_CLIP_mat4 = glGetUniformLocation(program, "OBJECT_TO_CLIP");

	GLuint TILE_TABLE_usampler2D = glGetUniformLocation(program, "TILE_TABLE");
	GLuint PALETTE_TABLE_sampler2D = glGetUniformLocation(program, "PALETTE_TABLE");

	//bind texture units indices to samplers:
	glUseProgram(program);
	glUniform1i(TILE_TABLE_usampler2D, 0);
	glUniform1i(PALETTE_TABLE_sampler2D, 1);
	glUseProgram(0);

	GL_ERRORS();
}

PPUTileProgram::~PPUTileProgram() {
	if (program != 0) {
		glDeleteProgram(program);
		program = 0;
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


//PPU data is streamed to the GPU (read: uploaded 'just in time') using a few buffers:
PPUDataStream::PPUDataStream() {

	//vertex_buffer_for_tile_program is a vertex array object that tells the GPU the layout of data in vertex_buffer:
	glGenVertexArrays(1, &vertex_buffer_for_tile_program);
	glBindVertexArray(vertex_buffer_for_tile_program);

	//vertex_buffer will (eventually) hold vertex data for drawing:
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	//Notice how this binding is attaching an integer input to a floating point attribute:
	glVertexAttribPointer(
		tile_program->Position_vec2, //attribute
		2, //size
		GL_INT, //type
		GL_FALSE, //normalized
		sizeof(Vertex), //stride
		(GLbyte *)0 + offsetof(Vertex, Position) //offset
	);
	glEnableVertexAttribArray(tile_program->Position_vec2);

	//the "I" variant binds to an integer attribute:
	glVertexAttribIPointer(
		tile_program->TileCoord_ivec2, //attribute
		2, //size
		GL_INT, //type
		sizeof(Vertex), //stride
		(GLbyte *)0 + offsetof(Vertex, TileCoord) //offset
	);
	glEnableVertexAttribArray(tile_program->TileCoord_ivec2);

	//I could have stored the Palette as another entry in the TileCoord attribute stream
	glVertexAttribIPointer(
		tile_program->Palette_int, //attribute
		1, //size
		GL_UNSIGNED_INT, //type
		sizeof(Vertex), //stride
		(GLbyte *)0 + offsetof(Vertex, Palette) //offset
	);
	glEnableVertexAttribArray(tile_program->Palette_int);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);


	glGenTextures(1, &tile_tex);
	glBindTexture(GL_TEXTURE_2D, tile_tex);
	//passing 'nullptr' to TexImage says "allocate memory but don't store anything there":
	// (textures will be uploaded later)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, 128, 128, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
	//make the texture have sharp pixels when magnified:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//when access past the edge, clamp to the edge:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);


	glGenTextures(1, &palette_tex);
	glBindTexture(GL_TEXTURE_2D, palette_tex);
	//passing 'nullptr' to TexImage says "allocate memory but don't store anything there":
	// (textures will be uploaded later)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 4, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	//make the texture have sharp pixels when magnified:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//when access past the edge, clamp to the edge:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);


	GL_ERRORS();
}

PPUDataStream::~PPUDataStream() {
	if (vertex_buffer_for_tile_program != 0) {
		glDeleteVertexArrays(1, &vertex_buffer_for_tile_program);
		vertex_buffer_for_tile_program = 0;
	}
	if (vertex_buffer != 0) {
		glDeleteBuffers(1, &vertex_buffer);
		vertex_buffer = 0;
	}
	if (tile_tex != 0) {
		glDeleteTextures(1, &tile_tex);
		tile_tex = 0;
	}
	if (palette_tex != 0) {
		glDeleteTextures(1, &palette_tex);
		palette_tex = 0;
	}
}
