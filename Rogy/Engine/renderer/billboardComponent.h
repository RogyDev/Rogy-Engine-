#ifndef BILLBOARD_COMPONENT_H
#define BILLBOARD_COMPONENT_H

#include "../scene/RComponent.h"
#include <glm/glm.hpp>
#include <string>

class BillboardComponent : public Component
{
	BASE_COMPONENT()
public:
	BillboardComponent();
	~BillboardComponent();

	glm::vec3 pos;
	glm::vec3 dir;
	glm::vec3 color;
	glm::vec2 size;
	bool depth_test;
	bool sun_source;
	bool use_tex_as_mask;
	std::string tex_path;

	// Serialization
	virtual void OnSave(YAML::Emitter& out) override
	{
		out << YAML::Key << "BillboardComponent" << YAML::BeginMap;

		out << YAML::Key << "enabled" << YAML::Value << enabled;
		out << YAML::Key << "sizex" << YAML::Value << size.x;
		out << YAML::Key << "sizey" << YAML::Value << size.y;
		out << YAML::Key << "depth_test" << YAML::Value << depth_test;
		out << YAML::Key << "sun_source" << YAML::Value << sun_source;
		out << YAML::Key << "tex_path" << YAML::Value << tex_path;
		out << YAML::Key << "color"; RYAML::SerVec3(out, color);
		out << YAML::Key << "use_tex_as_mask" << YAML::Value << use_tex_as_mask;

		out << YAML::EndMap;
	}

	virtual void OnLoad(YAML::Node& data) override
	{
		enabled = data["enabled"].as<bool>();
		size.x = data["sizex"].as<float>();
		size.y = data["sizey"].as<float>();
		depth_test = data["depth_test"].as<bool>();
		sun_source = data["sun_source"].as<bool>();
		tex_path = data["tex_path"].as<std::string>();
		color = RYAML::GetVec3(data["color"]);
		use_tex_as_mask = data["use_tex_as_mask"].as<bool>();
	}

};


#endif