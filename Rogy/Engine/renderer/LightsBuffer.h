#ifndef LIGHTS_BUFFER

#include "lighting.h"
#include "../shading/texture.h"

class LightsBuffer
{
	LightsBuffer() {}
	~LightsBuffer() {}

	void Init(unsigned int numberOfLights)
	{
		// Point light as defined in shader
		struct PLightStruct {
			glm::vec4 color; // color + intensity
			glm::vec4 position; // pos + raduis
			bool cast_shadows;
			int  shadow_index;
			float Bias;
		};

		numLights = numberOfLights;

		// Create the buffer
		glGenBuffers(1, &ubo_pLights);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo_pLights);
		glBufferData(GL_UNIFORM_BUFFER, numLights * sizeof(PLightStruct), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// define the range of the buffer that links to a uniform binding point
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo_pLights, 0, numLights * sizeof(PLightStruct));
	}

	void Update_pLights(std::vector<PointLight>* lights)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, ubo_pLights);

		for (size_t i = 0; i < numLights; i++)
		{
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), glm::value_ptr(lights[i]->color));
		}

		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	unsigned int numLights;
	unsigned int ubo_pLights;
};

#endif // !LIGHTS_BUFFER
