#ifndef MATERIAL_UNI_H
#define MATERIAL_UNI_H

#include <GL/glew.h>
#include <glm/glm.hpp>

enum Shader_Type
{
	Sh_PBR,
	Sh_Custom,
};

struct SS_preps
{
	GLuint proj;
	GLuint view;
	GLuint model;
	GLuint CamPos;

	glm::vec3 albedo;
	float metallic;
	float roughness;
	float ao;
};

#endif
