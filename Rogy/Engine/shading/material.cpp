#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <tuple>

#include <GL\glew.h>
#include <texture.h>
#include <shader.h>

#include "material.h"

Material::Material()
{
	albedo = glm::vec3(1.0f);
	ao = 1.0f;
	metallic = 0.0f;
	roughness = 0.5f;
	emission = glm::vec3(1.0f);
	emission_power = 1.0f;
}

Material::~Material()
{
}

void Material::setShader(Shader* shader)
{
	if (shader == nullptr)
		return;

	if (type == SHADER_PBR)
	{
		shader->SetVec3("albedo", albedo);
		shader->SetFloat("metallic", metallic);
		shader->SetFloat("roughness", roughness);
	}
}

std::string Material::getMatName()
{
	return name;
}

std::string Material::getMatPath()
{
	return path;
}

/*void Material::use()
{
	shader->use();

	if (mat_type == Sh_PBR)
	{
		shader->SetVec3("albedo", pbr_shader_prep.albedo);
		shader->SetFloat("metallic", pbr_shader_prep.metallic);
		shader->SetFloat("roughness", pbr_shader_prep.roughness);
		shader->SetFloat("ao", pbr_shader_prep.ao);
	}
}*/
