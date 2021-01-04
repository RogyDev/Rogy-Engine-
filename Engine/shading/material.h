#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <tuple>

#include <GL/glew.h>
#include <texture.h>
#include <shader.h>

enum Shader_Type
{
	SHADER_PBR,
	SHADER_CUSTOM,
};

class Material
{
public:
		Material();
		~Material();

		Shader_Type type;

		bool isDefault = false;

		glm::vec3 albedo;
		float metallic;
		float roughness;
		float ao;
		float specular = 0.5f;
		bool use_emission = false;
		glm::vec3 emission;
		float emission_power;
		glm::vec2 uv = glm::vec2(1.0f, 1.0f);

		Texture* tex_albedo = nullptr;
		Texture* tex_rough = nullptr;
		Texture* tex_metal = nullptr;
		Texture* tex_normal = nullptr;
		Texture* tex_emission = nullptr;

		int id;
		std::string name;
		std::string path;

		void setShader(Shader* shader);
		std::string getMatName();
		std::string getMatPath();

		// Serialization
		template <class Archive>
		void serialize(Archive & ar)
		{
			ar(name);
			ar(albedo.x, albedo.y, albedo.z);
			ar(metallic);
			ar(roughness);
			ar(ao);
			ar(uv.x, uv.y);

			ar(use_emission);
			if (use_emission)
			{
				ar(emission.x, emission.y, emission.z);
				ar(emission_power);
			}

			if (tex_albedo != nullptr)
			{
				ar(true);
				ar(tex_albedo->getTexName());
				ar(tex_albedo->getTexPath());
			}
			else { ar(false); }

			if (tex_normal != nullptr)
			{
				ar(true);
				ar(tex_normal->getTexName());
				ar(tex_normal->getTexPath());
			}
			else { ar(false); }

			if (tex_metal != nullptr)
			{
				ar(true);
				ar(tex_metal->getTexName());
				ar(tex_metal->getTexPath());
			}
			else { ar(false); }

			if (tex_rough != nullptr)
			{
				ar(true);
				ar(tex_rough->getTexName());
				ar(tex_rough->getTexPath());
			}
			else { ar(false); }

			if (tex_emission != nullptr)
			{
				ar(true);
				ar(tex_emission->getTexName());
				ar(tex_emission->getTexPath());
			}
			else { ar(false); }
		}
};

#endif
