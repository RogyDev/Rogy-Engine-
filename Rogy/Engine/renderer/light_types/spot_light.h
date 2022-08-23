#ifndef LIGHTING_SPOT_LIGHT_H
#define LIGHTING_SPOT_LIGHT_H
#include "glm\glm.hpp"
#include "../../scene/RComponent.h"

class SpotLight : public Component
{
public:
	static int TYPE_ID;

	int light_id;

	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Direction = glm::vec3(0.0f);
	glm::vec3 Color    = glm::vec3(1.0f);
	
	float CutOff    = 40.0f;
	float OuterCutOff = 5.0f;
	float Intensity = 10.0f;
	float Raduis    = 5.0f;
	float Bias = 0.005f;

	bool  CastShadows = false;

	bool  Static = false;
	bool  baked = false;

	int shadow_index = -1;
	bool visible = true;
	bool inFrustum = false;

	bool  Active = true; // is entity active? 

	bool isActive()
	{
		return (Active && enabled);
	}

	// Serialization
	virtual void OnSave(YAML::Emitter& out) override
	{
		out << YAML::Key << "SpotLight" << YAML::BeginMap;

		out << YAML::Key << "enabled" << YAML::Value << enabled;
		out << YAML::Key << "Color"; RYAML::SerVec3(out, Color);
		out << YAML::Key << "Bias" << YAML::Value << Bias;
		out << YAML::Key << "CastShadows" << YAML::Value << CastShadows;
		out << YAML::Key << "Intensity" << YAML::Value << Intensity;
		out << YAML::Key << "Raduis" << YAML::Value << Raduis;
		out << YAML::Key << "CutOff" << YAML::Value << CutOff;
		out << YAML::Key << "OuterCutOff" << YAML::Value << OuterCutOff;

		out << YAML::EndMap;
	}

	virtual void OnLoad(YAML::Node& data) override
	{
		enabled = data["enabled"].as<bool>();
		Color = RYAML::GetVec3(data["Color"]);
		Bias = data["Bias"].as<float>();
		CastShadows = data["CastShadows"].as<bool>();
		Intensity = data["Intensity"].as<float>();
		Raduis = data["Raduis"].as<float>();
		CutOff = data["CutOff"].as<float>();
		OuterCutOff = data["OuterCutOff"].as<float>();
	}
};

#endif