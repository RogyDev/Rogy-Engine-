#ifndef BBOX_H
#define BBOX_H
#include "glm\glm.hpp"

// Bounding Box
struct BBox
{
	glm::vec3 BoxMin = glm::vec3(-99999.0f);
	glm::vec3 BoxMax = glm::vec3(99999.0f);
	float radius;
	bool useRaduis = false;

	// AABB - AABB collision
	bool Intersect(BBox &target)
	{
		return (BoxMin.x <= target.BoxMax.x && BoxMax.x >= target.BoxMin.x) &&
			(BoxMin.y <= target.BoxMax.y && BoxMax.y >= target.BoxMin.y) &&
			(BoxMin.z <= target.BoxMax.z && BoxMax.z >= target.BoxMin.z);
	}

	// Serialization
	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(BoxMin.x, BoxMin.y, BoxMin.z);
		ar(BoxMax.x, BoxMax.y, BoxMax.z);
		ar(radius);
	}
	/*bool Intersect(BBox &a, BBox &b)
	{
		return (a.BoxMin.x <= b.BoxMax.x && a.BoxMax.x >= b.BoxMin.x) &&
			(a.BoxMin.y <= b.BoxMax.y && a.BoxMax.y >= b.BoxMin.y) &&
			(a.BoxMin.z <= b.BoxMax.z && a.BoxMax.z >= b.BoxMin.z);
	}*/
};
#endif