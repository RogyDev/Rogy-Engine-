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

	template <class Archive>
	void SerializeSave(Archive & ar)
	{
		ar(std::string("1.0"));
		ar(enabled);
		ar(size.x, size.y);
		ar(depth_test);
		ar(sun_source);
		ar(tex_path);
		ar(color.x, color.y, color.z);
		ar(use_tex_as_mask);
	}

	template <class Archive>
	void SerializeLoad(Archive & ar)
	{
		std::string ver;
		ar(ver);
		ar(enabled);
		ar(size.x, size.y);
		ar(depth_test);
		ar(sun_source);
		ar(tex_path);
		ar(color.x, color.y, color.z);
		ar(use_tex_as_mask);
	}
};


#endif