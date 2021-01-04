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
	// Serialization
	template <class Archive>
	void SerializeSave(Archive & ar)
	{
		ar(Color.x, Color.y, Color.z);
		ar(CastShadows);
		ar(Intensity);
		ar(Raduis);
		ar(Bias);
	}

	template <class Archive>
	void SerializeLoad(Archive & ar)
	{
		ar(Color.x, Color.y, Color.z);
		ar(CastShadows);
		ar(Intensity);
		ar(Raduis);
		ar(Bias);
	}
};

#endif