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
	template <class Archive>
	void SerializeSave(Archive & ar)
	{
		ar(box.BoxMin.x);
		ar(box.BoxMin.y);
		ar(box.BoxMin.z);

		ar(box.BoxMax.x);
		ar(box.BoxMax.y);
		ar(box.BoxMax.z);

		ar(box.radius);
		
		ar(Intensity);
		ar(static_only);
		ar(BoxProjection);
		ar(Resolution);
		ar(Intensity);
	}

private:
	BBox correct_box;
};

#endif // LIGHTING_REFLECTION_PROBE_H