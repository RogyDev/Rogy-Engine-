#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include "glm/glm.hpp"
#include "../mesh/model.h"
#include "../shading/material.h"
#include "../mesh/bounding_box.h"

/*

All render state required for submitting a render command.

*/
struct RenderCommand
{
	glm::mat4 Transform;
	Mesh*      Mesh;
	Material*  Material;
	bool cast_shadows;
	glm::vec3 position;
	BBox bbox;
	bool is_static;
	std::string lightmapPath;
};

#endif