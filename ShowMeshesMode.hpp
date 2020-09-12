#pragma once

/*
 *
 * ShowMeshesMode exists to show the contents of MeshBuffers; this can be useful
 * if, e.g., you aren't sure if things are being exported properly.
 *
 */

#include "Mode.hpp"
#include "Scene.hpp"
#include "Mesh.hpp"

struct ShowMeshesMode : Mode {
	ShowMeshesMode(MeshBuffer const &buffer);
	virtual ~ShowMeshesMode();

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

	//MeshBuffer being viewed:
	MeshBuffer const &buffer;

	//currently selected mesh:
	std::string current_mesh_name = "";
	glm::vec3 current_mesh_min = glm::vec3(0.0f);
	glm::vec3 current_mesh_max = glm::vec3(0.0f);
	void select_prev_mesh();
	void select_next_mesh();
	
	//Vertex array object used to bind mesh buffer for drawing:
	GLuint vao = 0;

	//mode uses a small Scene to arrange things for viewing:
	Scene scene;
	Scene::Camera *scene_camera = nullptr;
	Scene::Drawable *scene_drawable = nullptr;
};
