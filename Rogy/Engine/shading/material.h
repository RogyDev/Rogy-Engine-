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
#include <yaml-cpp/yaml.h>

enum Shader_Type
{
	SHADER_PBR,
	SHADER_Blend,
	SHADER_Laser,
	SHADER_Hightlight,
};

class Material
{
public:
		Material();
		~Material();

		Shader_Type type;

		bool isDefault = false;
		std::string tag;

		glm::vec3 albedo;
		float metallic;
		float roughness;
		float ao;
		float specular = 0.5f;
		bool use_emission = false;
		bool cutout = false;
		glm::vec3 emission;
		float emission_power;
		glm::vec2 uv = glm::vec2(1.0f, 1.0f);

		float val1;
		float val2;

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
		// ------------------------------------------------
		template <class Archive>
		void serialize(Archive & out)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Material" << YAML::Value << YAML::BeginSeq;
			out << YAML::BeginMap;
			out << YAML::Key << "type" << YAML::Value << (int)type;
			out << YAML::Key << "name" << YAML::Value << name;
			out << YAML::Key << "tag" << YAML::Value << tag;
			out << YAML::Key << "albedo_x" << YAML::Value << albedo.x;
			out << YAML::Key << "albedo_y" << YAML::Value << albedo.y;
			out << YAML::Key << "albedo_z" << YAML::Value << albedo.z;
			out << YAML::Key << "metallic" << YAML::Value << metallic;
			out << YAML::Key << "roughness" << YAML::Value << roughness;
			out << YAML::Key << "ao" << YAML::Value << ao;
			out << YAML::Key << "uv_x" << YAML::Value << uv.x;
			out << YAML::Key << "uv_y" << YAML::Value << uv.y;

			out << YAML::Key << "val1" << YAML::Value << val1;
			out << YAML::Key << "val2" << YAML::Value << val2;

			out << YAML::Key << "cutout" << YAML::Value << cutout;

			out << YAML::Key << "use_emission" << YAML::Value << use_emission;
			out << YAML::Key << "emission_x" << YAML::Value << emission.x;
			out << YAML::Key << "emission_y" << YAML::Value << emission.y;
			out << YAML::Key << "emission_z" << YAML::Value << emission.z;
			out << YAML::Key << "emission_power" << YAML::Value << emission_power;

			out << YAML::Key << "use_tex_albedo" << YAML::Value << (tex_albedo != nullptr);
			if (tex_albedo != nullptr)
			{
				out << YAML::Key << "tex_albedo_name" << YAML::Value << tex_albedo->getTexName();
				out << YAML::Key << "tex_albedo_source" << YAML::Value << tex_albedo->getTexPath();
			}

			out << YAML::Key << "use_tex_normal" << YAML::Value << (tex_normal != nullptr);
			if (tex_normal != nullptr)
			{
				out << YAML::Key << "tex_normal_name" << YAML::Value << tex_normal->getTexName();
				out << YAML::Key << "tex_normal_source" << YAML::Value << tex_normal->getTexPath();
			}

			out << YAML::Key << "use_tex_metal" << YAML::Value << (tex_metal != nullptr);
			if (tex_metal != nullptr)
			{
				out << YAML::Key << "tex_metal_name" << YAML::Value << tex_metal->getTexName();
				out << YAML::Key << "tex_metal_source" << YAML::Value << tex_metal->getTexPath();
			}

			out << YAML::Key << "use_tex_rough" << YAML::Value << (tex_rough != nullptr);
			if (tex_rough != nullptr)
			{
				out << YAML::Key << "tex_rough_name" << YAML::Value << tex_rough->getTexName();
				out << YAML::Key << "tex_rough_source" << YAML::Value << tex_rough->getTexPath();
			}

			out << YAML::Key << "use_tex_emission" << YAML::Value << (tex_emission != nullptr);
			if (tex_emission != nullptr)
			{
				out << YAML::Key << "tex_emission_name" << YAML::Value << tex_emission->getTexName();
				out << YAML::Key << "tex_emission_source" << YAML::Value << tex_emission->getTexPath();
			}

			out << YAML::EndMap;
			out << YAML::EndSeq;
			out << YAML::EndMap;
		}

		template <class Archive, class ResMng>
		void serializeLoad(Archive & out, ResMng* res)
		{
			auto& material_data = out["Material"];
			auto& data = material_data[0];
			if (data) {

				name = data["name"].as<std::string>();
				tag = data["tag"].as<std::string>();

				if(data["cutout"].IsDefined())
					cutout = data["cutout"].as<bool>();

				if (data["type"].IsDefined())
					type = (Shader_Type)data["type"].as<int>();

				if (data["val1"].IsDefined())
					val1 = data["val1"].as<float>();

				if (data["val2"].IsDefined())
					val1 = data["val2"].as<float>();

				albedo.x = data["albedo_x"].as<float>();
				albedo.y = data["albedo_y"].as<float>();
				albedo.z = data["albedo_z"].as<float>();
				metallic = data["metallic"].as<float>();
				roughness = data["roughness"].as<float>();
				ao = data["ao"].as<float>();
				uv.x = data["uv_x"].as<float>();
				uv.y = data["uv_y"].as<float>();

				use_emission = data["use_emission"].as<bool>();
				emission.x = data["emission_x"].as<float>();
				emission.y = data["emission_y"].as<float>();
				emission.z = data["emission_z"].as<float>();
				emission_power = data["emission_power"].as<float>();

				if (data["use_tex_albedo"].as<bool>())
				{
					tex_albedo = res->CreateTexture(data["tex_albedo_name"].as<std::string>(),
						data["tex_albedo_source"].as<std::string>().c_str());
				}

				if (data["use_tex_normal"].as<bool>())
				{
					tex_normal = res->CreateTexture(data["tex_normal_name"].as<std::string>(),
						data["tex_normal_source"].as<std::string>().c_str());
				}

				if (data["use_tex_metal"].as<bool>())
				{
					tex_metal = res->CreateTexture(data["tex_metal_name"].as<std::string>(),
						data["tex_metal_source"].as<std::string>().c_str());
				}

				if (data["use_tex_rough"].as<bool>())
				{
					tex_rough = res->CreateTexture(data["tex_rough_name"].as<std::string>(),
						data["tex_rough_source"].as<std::string>().c_str());
				}

				if (data["use_tex_emission"].as<bool>())
				{
					tex_emission = res->CreateTexture(data["tex_emission_name"].as<std::string>(),
						data["tex_emission_source"].as<std::string>().c_str());
				}
			}

		}
};

#endif
