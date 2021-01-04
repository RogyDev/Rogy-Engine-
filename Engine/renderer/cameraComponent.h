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

	template <class Archive>
	void SerializeSave(Archive & ar)
	{
		ar(std::string("1.0"));
		ar(FOV);
		ar(FarView);
		ar(NearView);
		ar(Primary);
	}

	template <class Archive>
	void SerializeLoad(Archive & ar)
	{
		std::string ver;
		ar(ver);
		ar(FOV);
		ar(FarView);
		ar(NearView);
		ar(Primary);
	}
};

#endif // !R_SCENE_CAMERA_H
