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
	float Bias        = 0.001f;
	bool  CastShadows = false;
	bool  Active	  = true;
	bool  Soft        = false;

	// Serialization
	template <class Archive>
	void SerializeSave(Archive & ar)
	{
		ar(Color.x, Color.y, Color.z);
		ar(Bias, Direction.y, Direction.z);
		ar(CastShadows);
		ar(Intensity);
		ar(Soft);
	}

	template <class Archive>
	void SerializeLoad(Archive & ar)
	{
		ar(Color.x, Color.y, Color.z);
		ar(Bias, Direction.y, Direction.z);
		ar(CastShadows);
		ar(Intensity);
		ar(Soft);
	}
};

#endif // LIGHTING_DIRECTIONAL_LIGHT_H