#ifndef LIGHTING_DIRECTIONAL_LIGHT_H
#define LIGHTING_DIRECTIONAL_LIGHT_H

#include "../../scene/RComponent.h"
#include "glm\glm.hpp"

class DirectionalLight : public Component
{
public:
	static int TYPE_ID;

	int light_id;

	glm::vec3 Direction = glm::vec3(0.0f);
	glm::vec3 Color = glm::vec3(1.0f);

	float Intensity   = 1.0f;
	float Bias        = 0.003f;
	bool  CastShadows = false;
	bool  Active	  = true;
	bool  Soft        = false;

	virtual void OnSave(YAML::Emitter& out) override
	{
		out << YAML::Key << "DirectionalLight" << YAML::BeginMap;

		out << YAML::Key << "Color"; RYAML::SerVec3(out, Color);
		out << YAML::Key << "Bias" << YAML::Value << Bias;
		out << YAML::Key << "CastShadows" << YAML::Value << CastShadows;
		out << YAML::Key << "Intensity" << YAML::Value << Intensity;
		out << YAML::Key << "Soft" << YAML::Value << Soft;

		out << YAML::EndMap;
	}

	virtual void OnLoad(YAML::Node& data) override
	{
		Color = RYAML::GetVec3(data["Color"]);
		Bias = data["Bias"].as<float>();
		CastShadows = data["CastShadows"].as<bool>();
		Intensity = data["Intensity"].as<float>();
		Soft = data["Soft"].as<bool>();
	}
};

#endif // LIGHTING_DIRECTIONAL_LIGHT_H