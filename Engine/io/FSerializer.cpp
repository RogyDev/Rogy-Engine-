#include "FSerializer.h"

void Vec3Bin(glm::vec3 t, cereal::BinaryOutputArchive& ar)
{	
	ar(t.x);
	ar(t.y);
	ar(t.z);
}
glm::vec3 Vec3Bin(cereal::BinaryInputArchive& ar)
{	
	glm::vec3 t;
	ar(t.x);
	ar(t.y);
	ar(t.z);
	return t;
}