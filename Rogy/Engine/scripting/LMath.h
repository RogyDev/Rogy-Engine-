#ifndef LUA_MATH_BINDING_H
#define LUA_MATH_BINDING_H

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/transform.hpp>

namespace LMath
{
	float Clamp(float val, float min, float max);

	glm::vec3 Lerp(glm::vec3 from, glm::vec3 to, float speed);

	float Abs(float a);
	float Sin(float a);
	float PI();
	float Sqrt(float val);

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);
}
#endif // LUA_MATH_BINDING_H