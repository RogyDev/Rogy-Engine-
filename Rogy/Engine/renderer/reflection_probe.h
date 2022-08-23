#ifndef LIGHTING_REFLECTION_PROBE_H
#define LIGHTING_REFLECTION_PROBE_H

#include "glm\glm.hpp"
#include "../mesh/bounding_box.h"
#include "pbr_capture.h"
#include "../scene/RComponent.h"

class ReflectionProbe : public Component
{
	BASE_COMPONENT()

	// Probe ID
	int probe_id;
	
	// Cubemap capture
	PBRCapture capture;

	// Local Bounding box - get the corrected b_box with GetBBox -
	BBox box;
	
	// Probe position
	glm::vec3 Position = glm::vec3(0.0f);

	// Intensity
	float Intensity  = 1.0f;
	// Capture Resolution
	float Resolution = 64.0f;

	// Project the reflections (corrected cubemap)
	bool  BoxProjection = false;
	bool  Active        = true;
	bool  removed	    = false;
	bool  static_only = true;
	bool  use_scale = true;

	// Is the reflection probe baked?
	bool  baked = false;

	void BakeReflections()
	{
		baked = false;
	}

	BBox GetBBox()
	{
		correct_box = box;
		correct_box.BoxMax += Position;
		correct_box.BoxMin += Position;
		return correct_box;
	}

	// Serialization
	virtual void OnSave(YAML::Emitter& out) override
	{
		out << YAML::Key << "ReflectionProbe" << YAML::BeginMap;

		out << YAML::Key << "box_BoxMin"; RYAML::SerVec3(out, box.BoxMin);
		out << YAML::Key << "box_BoxMax"; RYAML::SerVec3(out, box.BoxMax);
		out << YAML::Key << "box_radius" << YAML::Value << box.radius;
		out << YAML::Key << "Intensity" << YAML::Value << Intensity;
		out << YAML::Key << "static_only" << YAML::Value << static_only;
		out << YAML::Key << "BoxProjection" << YAML::Value << BoxProjection;
		out << YAML::Key << "Resolution" << YAML::Value << Resolution;
		out << YAML::Key << "use_scale" << YAML::Value << use_scale;

		out << YAML::EndMap;
	}

	virtual void OnLoad(YAML::Node& data) override
	{
		box.BoxMin = RYAML::GetVec3(data["box_BoxMin"]);
		box.BoxMax = RYAML::GetVec3(data["box_BoxMax"]);
		box.radius = data["box_radius"].as<float>();
		Intensity = data["Intensity"].as<float>();
		static_only = data["static_only"].as<bool>();
		BoxProjection = data["BoxProjection"].as<bool>();
		Resolution = data["Resolution"].as<float>();

		if (data["use_scale"].IsDefined())
			use_scale = data["use_scale"].as<bool>();
		else
			use_scale = false;
	}

private:
	BBox correct_box;
};

#endif // LIGHTING_REFLECTION_PROBE_H