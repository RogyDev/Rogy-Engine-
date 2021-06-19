#include "LMath.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace LMath
{
	float Clamp(float val, float min, float max)
	{
		if (val > max) return max;
		if (val < min) return min;
		return val;
	}

	float Sqrt(float val)
	{
		return glm::sqrt(val);
	}

	glm::vec3 Lerp(glm::vec3 from, glm::vec3 to, float speed)
	{
		return glm::lerp(from, to, speed);
	}

	float Abs(float a)
	{
		return glm::abs(a);
	}

	float Sin(float a)
	{
		return glm::sin(a);
	}

	float PI()
	{
		return 3.141592653f;
	}

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& Translation, glm::vec3& Rotation, glm::vec3& Scale)
	{
		using namespace glm;
		mat4 LocalMatrix(transform);

		// Next take care of translation (easy).
		Translation = vec3(LocalMatrix[3]);
		LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

		vec3 Row[3], Pdum3;

		// Now get scale and shear.
		for (length_t i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];

		// Compute X scale factor and normalize first row.
		Scale.x = length(Row[0]);// v3Length(Row[0]);

		v3Scale(Row[0], static_cast<float>(1));

		// Now, compute Y scale and normalize 2nd row.
		Scale.y = length(Row[1]);
		v3Scale(Row[1], static_cast<float>(1));

		// Next, get Z scale and normalize 3rd row.
		Scale.z = length(Row[2]);
		v3Scale(Row[2], static_cast<float>(1));

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (length_t i = 0; i < 3; i++)
			{
				Scale.x *= static_cast<float>(-1);
				Row[i] *= static_cast<float>(-1);
			}
		}

		return true;
	}
}