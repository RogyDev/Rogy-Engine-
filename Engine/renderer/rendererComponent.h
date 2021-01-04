#ifndef RENDERERCOMPONENT_H
#define RENDERERCOMPONENT_H

#include "../scene/RComponent.h"
#include <glm/glm.hpp>
#include <string>
#include "../mesh/model.h"
#include "../shading/material.h"

class RendererComponent : public Component
{
	BASE_COMPONENT()
	RendererComponent();
	~RendererComponent();

	glm::mat4 transform;
	glm::vec3 position;
	Mesh*      mesh     = nullptr;
	Material*  material = nullptr;
	bool IsStatic   = false;
	bool CastShadows = true;
	BBox bbox;
	std::string lightmapPath = "";

	template <class Archive>
	void SerializeSave(Archive & ar)
	{
		//ar(std::string("1.0"));
		ar(enabled);
		ar(CastShadows);
		ar(lightmapPath);
		if (mesh != nullptr) {
			ar(true);
			ar(mesh->path);
			ar(mesh->index);
		}
		else ar(false);
		ar(material->getMatName());
		ar(material->getMatPath());
	}
};


#endif // RENDERERCOMPONENT_H