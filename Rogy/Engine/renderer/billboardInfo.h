#ifndef R_BILLBOARD_INFO_H
#define R_BILLBOARD_INFO_H

#include "GL\glew.h"
#include "..\shading\material.h"
#include "glm\glm.hpp"
#include <iostream>
#include <vector>

struct ABillboardState
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 size;
	bool depth_test;
	bool sun_source;
	bool use_tex_as_mask;
	std::string tex_path;
	bool enabled;

	ABillboardState(glm::vec3 ppos, glm::vec2 ssize, bool use_depth, glm::vec3 col = glm::vec3(1.0f), bool tex_mask = false)
	{
		color = col;
		enabled = true;
		sun_source = false;
		use_tex_as_mask = tex_mask;
		pos = ppos;
		size = ssize;
		depth_test = use_depth;
		tex_path = std::string("core\\textures\\glow1.png");
	}

	template <class Archive>
	void serializeSave(Archive & ar)
	{
		ar(std::string("1.2"));
		ar(enabled);
		ar(size.x, size.y);
		ar(depth_test);
		ar(sun_source);
		ar(tex_path);
		ar(color.x, color.y, color.z);
		ar(use_tex_as_mask);
	}

	template <class Archive>
	void serializeLoad(Archive & ar)
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
class BillboardInfo
{
public:
	BillboardInfo() {}
	~BillboardInfo() {}
	unsigned int id;

	/* The billboard texture */
	Texture* texture;

	std::vector<ABillboardState> positions;

private:

};


#endif // !R_BILLBOARD_INFO_H
