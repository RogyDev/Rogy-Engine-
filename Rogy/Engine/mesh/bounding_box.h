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

	BBox(){}
	BBox(glm::vec3 min_, glm::vec3 max_)
	{
		BoxMin = min_;
		BoxMax = max_;
	}

	// Assign from another bounding box.
	BBox& operator =(const BBox& rhs)
	{
		BoxMin = rhs.BoxMin;
		BoxMax = rhs.BoxMax;
		return *this;
	}

	// AABB - AABB collision
	bool Intersect(BBox &target)
	{
		return (BoxMin.x <= target.BoxMax.x && BoxMax.x >= target.BoxMin.x) &&
			(BoxMin.y <= target.BoxMax.y && BoxMax.y >= target.BoxMin.y) &&
			(BoxMin.z <= target.BoxMax.z && BoxMax.z >= target.BoxMin.z);
	}

	// Return center.
	glm::vec3 Center() const { return (BoxMax + BoxMin) * 0.5f; }
	// Return size.
	glm::vec3 Size() const { return BoxMax - BoxMin; }

	BBox Transformed(glm::mat3 transform)
	{
		glm::vec3 newCenter = transform * Center();
		glm::vec3 oldEdge = Size() * 0.5f;
		glm::vec3 newEdge = glm::vec3(
			glm::abs(transform[0][0]) * oldEdge.x + glm::abs(transform[0][1]) * oldEdge.y + glm::abs(transform[0][2]) * oldEdge.z,
			glm::abs(transform[1][0]) * oldEdge.x + glm::abs(transform[1][1]) * oldEdge.y + glm::abs(transform[1][2]) * oldEdge.z,
			glm::abs(transform[2][0]) * oldEdge.x + glm::abs(transform[2][1]) * oldEdge.y + glm::abs(transform[2][2]) * oldEdge.z
		);
		
		return BBox(newCenter - newEdge, newCenter + newEdge);
	}

	void Transform(glm::mat4& tr, glm::vec3& pos)
	{
		glm::mat3 transform = (glm::mat3)tr;
		glm::vec3 newCenter = transform * Center();
		glm::vec3 oldEdge = Size() * 0.5f;
		glm::vec3 newEdge = glm::vec3(
			glm::abs(transform[0][0]) * oldEdge.x + glm::abs(transform[0][1]) * oldEdge.y + glm::abs(transform[0][2]) * oldEdge.z,
			glm::abs(transform[1][0]) * oldEdge.x + glm::abs(transform[1][1]) * oldEdge.y + glm::abs(transform[1][2]) * oldEdge.z,
			glm::abs(transform[2][0]) * oldEdge.x + glm::abs(transform[2][1]) * oldEdge.y + glm::abs(transform[2][2]) * oldEdge.z
		);
		//return BBox(newCenter - newEdge, newCenter + newEdge);
		BoxMin = (newCenter - newEdge) + pos;
		BoxMax = (newCenter + newEdge) + pos;
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