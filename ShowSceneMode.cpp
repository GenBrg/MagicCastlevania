#include "ShowSceneMode.hpp"
#include "DrawLines.hpp"

#include <iostream>

ShowSceneMode::ShowSceneMode(Scene const &scene_) : scene(scene_) {

	//Set up camera-only scene:
	{ //create a single camera:
		camera_scene.transforms.emplace_back();
		camera_scene.cameras.emplace_back(&camera_scene.transforms.back());
		scene_camera = &camera_scene.cameras.back();
		scene_camera->fovy = 60.0f / 180.0f * 3.1415926f;
		scene_camera->near = 0.01f;
		//scene_camera->transform and scene_camera->aspect will be set in draw()
	}
}

ShowSceneMode::~ShowSceneMode() {
}

bool ShowSceneMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
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

void ShowSceneMode::draw(glm::uvec2 const &drawable_size) {
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
		for (auto &transform : scene.transforms) {
			glm::mat4 local_to_world = transform.make_local_to_world();
			auto xf = [&local_to_world](glm::vec3 const &vec) {
				return glm::vec3(local_to_world * glm::vec4(vec, 1.0f));
			};
			auto xfd = [&local_to_world](glm::vec3 const &vec) {
				return glm::vec3(local_to_world * glm::vec4(vec, 0.0f));
			};

			if (transform.parent) {
				//connect to parent:
				glm::vec3 p = glm::vec3(transform.parent->make_local_to_world()[3]);
				draw_lines.draw(p, xf(glm::vec3(0.0f)), glm::u8vec4(0xff, 0xff, 0x00, 0xff));
			}


			//axis:
			float len = 0.2f;
			draw_lines.draw(xf(glm::vec3(0.0f)), xf(glm::vec3(len, 0.0f, 0.0f)), glm::u8vec4(0xff, 0x00, 0x00, 0xff));
			draw_lines.draw(xf(glm::vec3(0.0f)), xf(glm::vec3(-len, 0.0f, 0.0f)), glm::u8vec4(0x88, 0x00, 0x00, 0xff));
			draw_lines.draw(xf(glm::vec3(0.0f)), xf(glm::vec3(0.0f, len, 0.0f)), glm::u8vec4(0x00, 0xff, 0x00, 0xff));
			draw_lines.draw(xf(glm::vec3(0.0f)), xf(glm::vec3(0.0f, -len, 0.0f)), glm::u8vec4(0x00, 0x88, 0x00, 0xff));
			draw_lines.draw(xf(glm::vec3(0.0f)), xf(glm::vec3(0.0f, 0.0f, len)), glm::u8vec4(0x00, 0x00, 0xff, 0xff));
			draw_lines.draw(xf(glm::vec3(0.0f)), xf(glm::vec3(0.0f, 0.0f, -len)), glm::u8vec4(0x00, 0x00, 0x88, 0xff));

			//transform name:
			draw_lines.draw_text("'" + transform.name + "'",
				xf(glm::vec3(0.05f, 0.0f, 0.05f)),
				0.15f * xfd(glm::vec3(1.0f, 0.0f, 0.0f)),
				0.15f * xfd(glm::vec3(0.0f, 0.0f, 1.0f)),
				glm::u8vec4(0xff, 0xff, 0xff, 0xff)
			);
		}
		/*
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		*/
	}

}
