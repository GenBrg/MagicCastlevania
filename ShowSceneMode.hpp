#pragma once

/*
 *
 * ShowSceneMode exists to show the contents of a Scene; this can be useful
 * if, e.g., you aren't sure if things are being exported properly.
 *
 */

#include "Mode.hpp"
#include "Scene.hpp"
#include "Mesh.hpp"

struct ShowSceneMode : Mode {
	ShowSceneMode(Scene const &scene);
	virtual ~ShowSceneMode();

	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//z-up trackball-style camera controls:
	struct {
		float radius = 2.0f;
		float azimuth = 0.3f; //angle ccw of -y axis, in radians, [-pi,pi]
		float elevation = 0.2f; //angle above ground, in radians, [-pi,pi]
		glm::vec3 target = glm::vec3(0.0f);
		bool flip_x = false; //flip x inputs when moving? (used to handle situations where camera is upside-down)
	} camera;

	//Scene being viewed:
	Scene const &scene;

	//mode uses a secondary Scene to hold a camera:
	Scene camera_scene;
	Scene::Camera *scene_camera = nullptr;
};
