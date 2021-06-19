#ifndef R_SCENE_CAMERA_H
#define R_SCENE_CAMERA_H

#include "glm\glm.hpp"
#include "../scene/RComponent.h"

// struct that hold data about a camera component.
class CameraComponent : public Component
{
	BASE_COMPONENT()
public:
	float FOV = 60.0f;
	float FarView = 300.0f;
	float NearView = 0.001f;
	glm::vec3 position = glm::vec3(0.0f);
	//glm::vec3 rotationAngels = glm::vec3(0.0f);
	glm::vec3 up = glm::vec3(0.0f);
	glm::vec3 right = glm::vec3(0.0f);
	glm::vec3 direction = glm::vec3(0.0f);
	bool Primary = true;

	CameraComponent() {}
	~CameraComponent() {}

	virtual void OnSave(YAML::Emitter& out) override
	{
		out << YAML::Key << "CameraComponent" << YAML::BeginMap;

		out << YAML::Key << "FOV" << YAML::Value << FOV;
		out << YAML::Key << "FarView" << YAML::Value << FarView;
		out << YAML::Key << "NearView" << YAML::Value << NearView;
		out << YAML::Key << "Primary" << YAML::Value << Primary;

		out << YAML::EndMap;
	}

	virtual void OnLoad(YAML::Node& data) override
	{
		FOV = data["FOV"].as<float>();
		FarView = data["FarView"].as<float>();
		NearView = data["NearView"].as<float>();
		Primary = data["Primary"].as<bool>();
	}
};

#endif // !R_SCENE_CAMERA_H
