#include "ShowMeshesMode.hpp"

#include "ShowMeshesProgram.hpp"
#include "DrawLines.hpp"

#include <iostream>

ShowMeshesMode::ShowMeshesMode(MeshBuffer const &buffer_) : buffer(buffer_) {
	vao = buffer.make_vao_for_program(show_meshes_program->program);

	//Set up scene:
	{ //create a single camera:
		scene.transforms.emplace_back();
		scene.cameras.emplace_back(&scene.transforms.back());
		scene_camera = &scene.cameras.back();
		scene_camera->fovy = 60.0f / 180.0f * 3.1415926f;
		scene_camera->near = 0.01f;
		//scene_camera->transform and scene_camera->aspect will be set in draw()
	}
	{ //create a drawable to hold the current mesh:
		scene.transforms.emplace_back();
		scene.drawables.emplace_back(&scene.transforms.back());
		scene_drawable = &scene.drawables.back();

		scene_drawable->pipeline = show_meshes_program_pipeline;
		scene_drawable->pipeline.vao = vao;
		//these will be updated by the mesh selection code:
		scene_drawable->pipeline.type = GL_TRIANGLES;
		scene_drawable->pipeline.start = 0;
		scene_drawable->pipeline.count = 0;
	}

	//select first mesh in buffer:
	select_prev_mesh();
}

ShowMeshesMode::~ShowMeshesMode() {
}

bool ShowMeshesMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_RIGHT) {
			select_next_mesh();
			return true;
		}
		if (evt.key.keysym.sym == SDLK_LEFT) {
			select_prev_mesh();
			return true;
		}
	}

	//----- trackball-style camera controls -----
	if (evt.type == SDL_MOUSEBUTTONDOWN) {
		if (evt.button.button == SDL_BUTTON_LEFT) {
			//when camera is upside-down at rotation start, azimuth rotation should be reversed:
			// (this ends up feeling more intuitive)
			camera.flip_x = (std::abs(camera.elevation) > 0.5f * 3.1415926f);
			return true;
		}
	}
	if (evt.type == SDL_MOUSEMOTION) {
		if (evt.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
			//figure out the motion (as a fraction of a normalized [-a,a]x[-1,1] window):
			glm::vec2 delta;
			delta.x = evt.motion.xrel / float(window_size.x) * 2.0f;
			delta.x *= float(window_size.y) / float(window_size.x);
			delta.y = evt.motion.yrel / float(window_size.y) * -2.0f;

			if (SDL_GetModState() & KMOD_SHIFT) {
				//shift: pan

				glm::mat3 frame = glm::mat3_cast(scene_camera->transform->rotation);
				camera.target -= frame[0] * (delta.x * camera.radius) + frame[1] * (delta.y * camera.radius);
			} else {
				//no shift: tumble

				camera.azimuth -= 3.0f * delta.x * (camera.flip_x ? -1.0f : 1.0f);
				camera.elevation -= 3.0f * delta.y;

				camera.azimuth /= 2.0f * 3.1415926f;
				camera.azimuth -= std::round(camera.azimuth);
				camera.azimuth *= 2.0f * 3.1415926f;

				camera.elevation /= 2.0f * 3.1415926f;
				camera.elevation -= std::round(camera.elevation);
				camera.elevation *= 2.0f * 3.1415926f;

				//std::cout << camera.azimuth / 3.1415926f * 180.0f << " / " << camera.elevation / 3.1415926f * 180.0f << std::endl;
			}
			return true;
		}
	}
	//mouse wheel: dolly
	if (evt.type == SDL_MOUSEWHEEL) {
		camera.radius *= std::pow(0.5f, 0.1f * evt.wheel.y);
		if (camera.radius < 1e-1f) camera.radius = 1e-1f;
		if (camera.radius > 1e6f) camera.radius = 1e6f;
		return true;
	}
	
	return false;
}

void ShowMeshesMode::draw(glm::uvec2 const &drawable_size) {
	//--- use camera structure to set up scene camera ---

	scene_camera->transform->rotation =
		glm::angleAxis(camera.azimuth, glm::vec3(0.0f, 0.0f, 1.0f))
		* glm::angleAxis(0.5f * 3.1415926f + -camera.elevation, glm::vec3(1.0f, 0.0f, 0.0f))
	;
	scene_camera->transform->position = camera.target + camera.radius * (scene_camera->transform->rotation * glm::vec3(0.0f, 0.0f, 1.0f));
	scene_camera->transform->scale = glm::vec3(1.0f);
	scene_camera->aspect = float(drawable_size.x) / float(drawable_size.y);


	//--- actual drawing ---
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	scene.draw(*scene_camera);

	{ //decorate with some lines:
		DrawLines draw_lines(scene_camera->make_projection() * glm::mat4(scene_camera->transform->make_world_to_local()));

		//axis (unit-length):
		draw_lines.draw(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::u8vec4(0xff, 0x00, 0x00, 0xff));
		draw_lines.draw(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::u8vec4(0x00, 0xff, 0x00, 0xff));
		draw_lines.draw(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::u8vec4(0x00, 0x00, 0xff, 0xff));

		//bounding box:
		glm::vec3 r = 0.5f * (current_mesh_max - current_mesh_min);
		glm::vec3 c = 0.5f * (current_mesh_max + current_mesh_min);
		glm::mat4x3 mat(
			glm::vec3(r.x,  0.0f, 0.0f),
			glm::vec3(0.0f,  r.y, 0.0f),
			glm::vec3(0.0f, 0.0f,  r.z),
			c
		);
		draw_lines.draw_box(mat, glm::u8vec4(0xdd, 0xdd, 0xdd, 0xff));

		//mesh name:
		draw_lines.draw_text("'" + current_mesh_name + "'",
			current_mesh_min + glm::vec3(0.0f, -0.20f, 0.0f),
			0.15f * glm::vec3(1.0f, 0.0f, 0.0f),
			0.15f * glm::vec3(0.0f, 1.0f, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0xff)
		);
	}
}

void ShowMeshesMode::select_prev_mesh() {
	auto f = buffer.meshes.find(current_mesh_name);
	if (f != buffer.meshes.end()) --f;
	if (f == buffer.meshes.end()) f = buffer.meshes.begin();

	if (f != buffer.meshes.end()) {
		current_mesh_name = f->first;
		scene_drawable->pipeline.type = f->second.type;
		scene_drawable->pipeline.start = f->second.start;
		scene_drawable->pipeline.count = f->second.count;
		current_mesh_min = f->second.min;
		current_mesh_max = f->second.max;
	} else {
		current_mesh_name = "";
		scene_drawable->pipeline.type = GL_TRIANGLES;
		scene_drawable->pipeline.start = 0;
		scene_drawable->pipeline.count = 0;
		current_mesh_min = glm::vec3(0.0f);
		current_mesh_max = glm::vec3(0.0f);
	}
}

void ShowMeshesMode::select_next_mesh() {
	auto f = buffer.meshes.find(current_mesh_name);
	if (f != buffer.meshes.end()) ++f;
	if (f == buffer.meshes.end()) {
		auto temp = buffer.meshes.rbegin();
		if (temp != buffer.meshes.rend()) {
			++temp;
			f = temp.base();
			assert(f != buffer.meshes.end());
		}
	}

	if (f != buffer.meshes.end()) {
		current_mesh_name = f->first;
		scene_drawable->pipeline.type = f->second.type;
		scene_drawable->pipeline.start = f->second.start;
		scene_drawable->pipeline.count = f->second.count;
		current_mesh_min = f->second.min;
		current_mesh_max = f->second.max;
	} else {
		current_mesh_name = "";
		scene_drawable->pipeline.type = GL_TRIANGLES;
		scene_drawable->pipeline.start = 0;
		scene_drawable->pipeline.count = 0;
		current_mesh_min = glm::vec3(0.0f);
		current_mesh_max = glm::vec3(0.0f);
	}
}
