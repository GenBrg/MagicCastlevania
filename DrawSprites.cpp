#include "DrawSprites.hpp"

#include "ColorTextureProgram.hpp"
#include "Load.hpp"

#include "GL.hpp"
#include "gl_errors.hpp"
#include "Util.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

//for glm::to_string():
#include <glm/gtx/string_cast.hpp>

//All DrawSprites instances share a vertex array object and vertex buffer, initialized at load time:

//n.b. declared static so they don't conflict with similarly named global variables elsewhere:
static GLuint vertex_buffer = 0;
static GLuint vertex_buffer_for_color_texture_program = 0;

Load< void > setup_buffers(LoadTagDefault, [](){
	//you may recognize this init code from PongMode.cpp in base0:

	{ //set up vertex buffer:
		glGenBuffers(1, &vertex_buffer);
		//for now, buffer will be un-filled.
	}

	{ //vertex array mapping buffer for color_texture_program:
		//ask OpenGL to fill vertex_buffer_for_color_texture_program with the name of an unused vertex array object:
		glGenVertexArrays(1, &vertex_buffer_for_color_texture_program);

		//set vertex_buffer_for_color_texture_program as the current vertex array object:
		glBindVertexArray(vertex_buffer_for_color_texture_program);

		//set vertex_buffer as the source of glVertexAttribPointer() commands:
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

		//set up the vertex array object to describe arrays of PongMode::Vertex:
		glVertexAttribPointer(
			color_texture_program->Position_vec4, //attribute
			2, //size
			GL_FLOAT, //type
			GL_FALSE, //normalized
			sizeof(DrawSprites::Vertex), //stride
			(GLbyte *)0 + offsetof(DrawSprites::Vertex, Position) //offset
		);
		glEnableVertexAttribArray(color_texture_program->Position_vec4);
		//[Note that it is okay to bind a vec3 input to a vec4 attribute -- the w component will be filled with 1.0 automatically]

		glVertexAttribPointer(
			color_texture_program->TexCoord_vec2, //attribute
			2, //size
			GL_FLOAT, //type
			GL_FALSE, //normalized
			sizeof(DrawSprites::Vertex), //stride
			(GLbyte *)0 + offsetof(DrawSprites::Vertex, TexCoord) //offset
		);
		glEnableVertexAttribArray(color_texture_program->TexCoord_vec2);

		glVertexAttribPointer(
			color_texture_program->Color_vec4, //attribute
			4, //size
			GL_UNSIGNED_BYTE, //type
			GL_TRUE, //normalized
			sizeof(DrawSprites::Vertex), //stride
			(GLbyte *)0 + offsetof(DrawSprites::Vertex, Color) //offset
		);
		glEnableVertexAttribArray(color_texture_program->Color_vec4);

		//done referring to vertex_buffer, so unbind it:
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//done setting up vertex array object, so unbind it:
		glBindVertexArray(0);
	}

	GL_ERRORS(); //PARANOIA: make sure nothing strange happened during setup
});


DrawSprites::DrawSprites(
	SpriteAtlas const &atlas_,
	glm::vec2 const &view_min_, glm::vec2 const &view_max_,
	glm::uvec2 const &drawable_size_,
	DrawSprites::AlignMode mode_
	) :
	atlas(atlas_),
	view_min(view_min_), view_max(view_max_),
	drawable_size(drawable_size_),
	mode(mode_) {

	glm::vec2 window_min, window_max;
	//if view_size.x / view_size.y < drawable_size.x / drawable_size.y...
	if ((view_max.x - view_min.x) * drawable_size.y < drawable_size.x * (view_max.y - view_min.y)) {
		//...need to stretch wider to match aspect:
		float w = (view_max.y - view_min.y) * float(drawable_size.x) / float(drawable_size.y);
		window_min.x = 0.5f * (view_min.x + view_max.x) - 0.5f * w;
		window_max.x = 0.5f * (view_min.x + view_max.x) + 0.5f * w;
		window_min.y = view_min.y;
		window_max.y = view_max.y;
	} else {
		//...need to stretch taller to match aspect:
		window_min.x = view_min.x;
		window_max.x = view_max.x;
		float h = (view_max.x - view_min.x) * float(drawable_size.y) / float(drawable_size.x);
		window_min.y = 0.5f * (view_min.y + view_max.y) - 0.5f * h;
		window_max.y = 0.5f * (view_min.y + view_max.y) + 0.5f * h;
	}

	glm::vec2 scale = glm::vec2(2.0f) / (window_max - window_min);
	to_clip = glm::mat4( //n.b. column major(!)
		scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		scale.x * -0.5f * (window_min.x + window_max.x), scale.y * -0.5f * (window_min.y + window_max.y), 0.0f, 1.0f
	);
	//DEBUG: std::cout << glm::to_string(to_clip) << std::endl;
}
void DrawSprites::draw_bounding_box(glm::vec2 const lower_left_corner, glm::vec2 const upper_right_corner) {
	Transform2D box_transform = Transform2D(nullptr);
	box_transform.position_ = glm::vec2(lower_left_corner.x, lower_left_corner.y);
	box_transform.scale_.x = (upper_right_corner.x - lower_left_corner.x) / 100.0f;
	draw(sprites->lookup("bounding_edge_horizontal"), box_transform);
	box_transform.position_ = glm::vec2(lower_left_corner.x, upper_right_corner.y);
	draw(sprites->lookup("bounding_edge_horizontal"), box_transform);
	box_transform.scale_.x = 1.0f;
	box_transform.scale_.y = (upper_right_corner.y - lower_left_corner.y) / 100.0f;;
	box_transform.position_ = glm::vec2(lower_left_corner.x, lower_left_corner.y);
	draw(sprites->lookup("bounding_edge_vertical"), box_transform);
	box_transform.position_ = glm::vec2(upper_right_corner.x, lower_left_corner.y);
	draw(sprites->lookup("bounding_edge_vertical"), box_transform);
	//std::cout << "Drawing bounding box at (" << lower_left_corner.x << "," << lower_left_corner.y << ") (" << upper_right_corner.x << "," << upper_right_corner.y << ")" << std::endl;
}
void DrawSprites::draw(Sprite const &sprite, const Transform2D& transform, glm::u8vec4 const &tint) {
	glm::vec2 min_tc = sprite.min_px / glm::vec2(atlas.tex_sizes[sprite.atlas_idx]);
	glm::vec2 max_tc = sprite.max_px / glm::vec2(atlas.tex_sizes[sprite.atlas_idx]);

	glm::vec2 lower_left_corner = sprite.min_px - sprite.anchor_px;
	glm::vec2 upper_right_corner = sprite.max_px - sprite.anchor_px;

	// Model
	glm::vec2 corners[4] {
		lower_left_corner,
		glm::vec2(upper_right_corner.x, lower_left_corner.y),
		glm::vec2(lower_left_corner.x, upper_right_corner.y),
		upper_right_corner
	};

	glm::mat3 model_mat = transform.MakeLocalToWorld();
	
	for (auto& corner : corners) {
		corner = model_mat * glm::vec3(corner, 1.0f);
	}

	if (mode == AlignPixelPerfect) {
		//TODO: nudge min/max so that pixels line just ~just so~
	}

	//you may recognize this from draw_rectangle in base0:
	//split rectangle into two triangles:
	std::vector<Vertex>& attribs = tex_attribs_map[atlas.texes[sprite.atlas_idx]];
	attribs.emplace_back(corners[0], glm::vec2(min_tc.x,min_tc.y), tint);
	attribs.emplace_back(corners[1], glm::vec2(max_tc.x,min_tc.y), tint);
	attribs.emplace_back(corners[3], glm::vec2(max_tc.x,max_tc.y), tint);

	attribs.emplace_back(corners[0], glm::vec2(min_tc.x,min_tc.y), tint);
	attribs.emplace_back(corners[3], glm::vec2(max_tc.x,max_tc.y), tint);
	attribs.emplace_back(corners[2], glm::vec2(min_tc.x,max_tc.y), tint);

}

DrawSprites::~DrawSprites() {
	if (tex_attribs_map.empty()) return;

	//based on base0's PongMode::draw()

	for (GLuint tex : atlas.texes) {
		auto& attribs = tex_attribs_map[tex];
		
		//upload vertices to vertex_buffer:
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); //set vertex_buffer as current
		glBufferData(GL_ARRAY_BUFFER, attribs.size() * sizeof(attribs[0]), attribs.data(), GL_STREAM_DRAW); //upload attribs array
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//set color_texture_program as current program:
		glUseProgram(color_texture_program->program);

		//upload OBJECT_TO_CLIP to the proper uniform location:
		glUniformMatrix4fv(color_texture_program->OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(to_clip));

		//use the mapping vertex_buffer_for_color_texture_program to fetch vertex data:
		glBindVertexArray(vertex_buffer_for_color_texture_program);

		//bind the solid white texture to location zero:
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);

		//run the OpenGL pipeline:
		glDrawArrays(GL_TRIANGLES, 0, GLsizei(attribs.size()));

		//unbind the solid white texture:
		glBindTexture(GL_TEXTURE_2D, 0);

		//reset vertex array to none:
		glBindVertexArray(0);

		//reset current program to none:
		glUseProgram(0);
	}
}

