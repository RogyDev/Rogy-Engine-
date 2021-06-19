#ifndef LIGHTING_POINT_LIGHT_H
#define LIGHTING_POINT_LIGHT_H
#include "glm\glm.hpp"
#include "../../scene/RComponent.h"

enum ShadowCast
{
	Realtime = 0,
	Once = 1
};
class PointLight : public Component
{
public:
	static int TYPE_ID;

	int light_id;

	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Color    = glm::vec3(1.0f);

	float Intensity = 10.0f;
	float Raduis    = 5.0f;
	float Bias      = 0.05f;

	bool  CastShadows = false;
	bool  Active      = true;
	bool  Static = false;
	bool baked = false;

	unsigned int shadow_index = -1;
	bool visible = true;
	bool inFrustum = false;

	// Serialization
	virtual void OnSave(YAML::Emitter& out) override
	{
		out << YAML::Key << "PointLight" << YAML::BeginMap;

		out << YAML::Key << "Color"; RYAML::SerVec3(out, Color);
		out << YAML::Key << "Bias" << YAML::Value << Bias;
		out << YAML::Key << "CastShadows" << YAML::Value << CastShadows;
		out << YAML::Key << "Intensity" << YAML::Value << Intensity;
		out << YAML::Key << "Raduis" << YAML::Value << Raduis;

		out << YAML::EndMap;
	}

	virtual void OnLoad(YAML::Node& data) override
	{
		Color = RYAML::GetVec3(data["Color"]);
		Bias = data["Bias"].as<float>();
		CastShadows = data["CastShadows"].as<bool>();
		Intensity = data["Intensity"].as<float>();
		Raduis = data["Raduis"].as<float>();
	}
};

#endif