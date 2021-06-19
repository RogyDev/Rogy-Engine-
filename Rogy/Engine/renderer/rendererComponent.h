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

	// Serialization
	virtual void OnSave(YAML::Emitter& out) override
	{
		out << YAML::Key << "RendererComponent" << YAML::BeginMap;

		out << YAML::Key << "enabled" << YAML::Value << enabled;
		out << YAML::Key << "CastShadows" << YAML::Value << CastShadows;
		out << YAML::Key << "lightmapPath" << YAML::Value << lightmapPath;
		// getMatPath and getMatName are fliped here because
		// name and path are fliped in the material (will be fixed).
		out << YAML::Key << "mat_name" << YAML::Value << material->getMatPath();
		out << YAML::Key << "mat_source" << YAML::Value << material->getMatName();
		out << YAML::Key << "HasMesh" << YAML::Value << (mesh != nullptr);
		if (mesh != nullptr) {
			out << YAML::Key << "mesh_source" << YAML::Value << mesh->path;
			out << YAML::Key << "mesh_index" << YAML::Value << mesh->index;
		}

		out << YAML::EndMap;
	}

	virtual void OnLoad(YAML::Node& data) override
	{
		enabled = data["enabled"].as<bool>();
		CastShadows = data["CastShadows"].as<bool>();
		lightmapPath = data["lightmapPath"].as<std::string>();
	}
};


#endif // RENDERERCOMPONENT_H