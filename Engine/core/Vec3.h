#ifndef R_VECTOR_HELPERS_H
#define R_VECTOR_HELPERS_H

#include <glm\glm.hpp>
#include <iostream>

// glm::vec3 lua binding helper.
// ------------------------------------------
struct Vec3Helper
{
	template<unsigned index>
	static float get(glm::vec3 const* vec)
	{
		if(index == 0)
			return vec->x;
		if (index == 1)
			return vec->y;

		return vec->z;
	}

	template<unsigned index>
	static void set(glm::vec3* vec, float value)
	{
		if (index == 0){
			vec->x = value;
			return;
		}
		if (index == 1) {
			vec->y = value;
			return;
		}

		vec->z = value;
	}

	// Operators
	// -----------------------------------------
	static glm::vec3* ADD(glm::vec3* vec, glm::vec3* vecTow)
	{
		vec->x += vecTow->x;
		vec->y += vecTow->y;
		vec->z += vecTow->z;
		return vec;
	}
	static glm::vec3* MUL(glm::vec3* vec, glm::vec3* vecTow)
	{
		vec->x *= vecTow->x;
		vec->y *= vecTow->y;
		vec->z *= vecTow->z;
		return vec;
	}
	static glm::vec3* DIV(glm::vec3* vec, glm::vec3* vecTow)
	{
		vec->x /= vecTow->x;
		vec->y /= vecTow->y;
		vec->z /= vecTow->z;
		return vec;
	}
	static glm::vec3* SUB(glm::vec3* vec, glm::vec3* vecTow)
	{
		vec->x -= vecTow->x;
		vec->y -= vecTow->y;
		vec->z -= vecTow->z;
		return vec;
	}

	static bool lessThen(glm::vec3* a, glm::vec3* b)
	{
		return (a < b);
	}

	static bool LessOrEq(glm::vec3* a, glm::vec3* b)
	{
		return (a <= b);
	}

	static bool Equale(glm::vec3* a, glm::vec3* b)
	{
		return (a == b);
	}

	static glm::vec3 VecUp()
	{
		return glm::vec3(0.0f, 1.0f, 0.0f);
	}

	static glm::vec3 VecRight()
	{
		return glm::vec3(1.0f, 0.0f, 0.0f);
	}

	static glm::vec3 VecForward()
	{
		return glm::vec3(0.0f, 0.0f, 1.0f);
	}

	static glm::vec3 Vec3F(float val)
	{
		return glm::vec3(val, val, val);
	}

	static glm::vec3 Lerp(glm::vec3 a, glm::vec3 b, float d)
	{
		return glm::lerp(a, b, d);
	}
};


// glm::vec2 lua binding helper.
// ------------------------------------------
struct Vec2Helper
{
	template<unsigned index>
	static float get(glm::vec2 const* vec)
	{
		if (index == 0)
			return vec->x;

		return vec->y;
	}

	template<unsigned index>
	static void set(glm::vec2* vec, float value)
	{
		if (index == 0) {
			vec->x = value;
			return;
		}
		vec->y = value;
	}

	// Operators
	// -----------------------------------------
	static glm::vec2* ADD(glm::vec2* vec, glm::vec2* vecTow)
	{
		vec->x += vecTow->x;
		vec->y += vecTow->y;
		return vec;
	}
	static glm::vec2* MUL(glm::vec2* vec, glm::vec2* vecTow)
	{
		vec->x *= vecTow->x;
		vec->y *= vecTow->y;
		return vec;
	}
	static glm::vec2* DIV(glm::vec2* vec, glm::vec2* vecTow)
	{
		vec->x /= vecTow->x;
		vec->y /= vecTow->y;
		return vec;
	}
	static glm::vec2* SUB(glm::vec2* vec, glm::vec2* vecTow)
	{
		vec->x -= vecTow->x;
		vec->y -= vecTow->y;
		return vec;
	}

	static bool lessThen(glm::vec2* a, glm::vec2* b)
	{
		return (a < b);
	}

	static bool LessOrEq(glm::vec2* a, glm::vec2* b)
	{
		return (a <= b);
	}

	static bool Equale(glm::vec2* a, glm::vec2* b)
	{
		return (a == b);
	}

	static glm::vec2 VecUp()
	{
		return glm::vec2(0.0f, 1.0f);
	}

	static glm::vec2 VecRight()
	{
		return glm::vec2(1.0f, 0.0f);
	}

	static glm::vec2 Vec2F(float val)
	{
		return glm::vec2(val, val);
	}

	static glm::vec2 Lerp(glm::vec2 a, glm::vec2 b, float d)
	{
		return glm::lerp(a, b, d);
	}
};

#endif // ! R_VECTOR_HELPERS_H