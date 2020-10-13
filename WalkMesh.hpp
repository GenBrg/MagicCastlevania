#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp> //allows the use of 'uvec2' as an unordered_map key

#include <vector>
#include <string>
#include <unordered_map>

//"WalkPoint" represents location on the WalkMesh as barycentric coordinates on a triangle:
struct WalkPoint {
	//indices of current triangle (in CCW order):
	glm::uvec3 indices = glm::uvec3(-1U);
	//barycentric coordinates for current point:
	glm::vec3 weights = glm::vec3(std::numeric_limits< float >::quiet_NaN());
	//NOTE: by convention, if WalkPoint is on an edge, indices/weights will be arranged so that weights.z will be 0.0.
	WalkPoint(glm::uvec3 const &indices_, glm::vec3 const &weights_) : indices(indices_), weights(weights_) { }
	WalkPoint() = default;
};

struct WalkMesh {
	//Walk mesh will keep track of triangles, vertices:
	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec3 > normals; //normals for interpolated 'up' direction
	std::vector< glm::uvec3 > triangles; //CCW-oriented

	//This "next vertex" map includes [a,b]->c, [b,c]->a, and [c,a]->b for each triangle (a,b,c), and is useful for checking what's over an edge from a given point:
	std::unordered_map< glm::uvec2, uint32_t > next_vertex;

	//Construct new WalkMesh and build next_vertex structure:
	WalkMesh(std::vector< glm::vec3 > const &vertices_, std::vector< glm::vec3 > const &normals_, std::vector< glm::uvec3 > const &triangles_);

	//used to initialize walking -- finds the closest point on the walk mesh:
	// (should only need to call this at the start of a level)
	WalkPoint nearest_walk_point(glm::vec3 const &world_point) const;


	//take a step on a triangle, stopping at edges:
	//  if the step stays within the triangle:
	//   - *end will be the position after stepping
	//   - *remaining_step will be glm::vec3(0.0)
	//  if the step reaches a triangle edge:
	//   - *end will be a position along the edge (i.e., end->weights.z == 0.0f)
	//   - *remaining_step will contain the amount of step remaining
	void walk_in_triangle(
		WalkPoint const &start,   //[in] starting location on triangle
		glm::vec3 const &step,    //[in] step to take (in world space). Will be projected to triangle.
		WalkPoint *end,           //[out] final position in triangle
		float *time               //[out] time at which edge is encountered, or 1.0 if whole step is within triangle
	) const;

	//traverse over a triangle edge, adjusting facing direction
	//  if edge is a boundary edge:
	//    - *end gets start
	//    - *rotation is the identity
	//    - function returns false
	//  if edge is an internal edge:
	//    - end->weights is the other triangle along start.triangle.xy
	//    - *rotation brings vectors in the plane of start.triangle.xyz to vectors in the plane of end->triangle.xyz
	//    - function returns true
	bool cross_edge(
		WalkPoint const &start, //[in] walkpoint on triangle edge
		WalkPoint *end,         //[out] end walkpoint, having crossed edge
		glm::quat *rotation     //[out] rotation over edge
	) const;

	//used to read back results of walking:
	glm::vec3 to_world_point(WalkPoint const &wp) const {
		//if you were looking here for the lesson solution, well, here you go:
		// (but please do make sure you understand what this is doing)
		return wp.weights.x * vertices[wp.indices.x]
		     + wp.weights.y * vertices[wp.indices.y]
		     + wp.weights.z * vertices[wp.indices.z];
	}

	//read back a smoothed normal (average of vertex normals) at a walkpoint:
	glm::vec3 to_world_smooth_normal(WalkPoint const &wp) const {
		return glm::normalize(
			  wp.weights.x * normals[wp.indices.x]
			+ wp.weights.y * normals[wp.indices.y]
			+ wp.weights.z * normals[wp.indices.z]
		);
	}

	//read back a triangle normal at a walkpoint:
	glm::vec3 to_world_triangle_normal(WalkPoint const &wp) const {
		glm::vec3 const &a = vertices[wp.indices.x];
		glm::vec3 const &b = vertices[wp.indices.y];
		glm::vec3 const &c = vertices[wp.indices.z];
		return glm::normalize( glm::cross( b-a, c-a ) );
	}

};

struct WalkMeshes {
	//load a list of named WalkMeshes from a file:
	WalkMeshes(std::string const &filename);

	//retrieve a WalkMesh by name:
	WalkMesh const &lookup(std::string const &name) const;

	//internals:
	std::unordered_map< std::string, WalkMesh > meshes;
};
