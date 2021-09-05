#ifndef FORWARD_PLUS_RENDERER
#define FORWARD_PLUS_RENDERER

#include "../camera/Camera.h"
#include "lighting.h"
#include <vector>
#include <gl\glew.h>
#include <string>
//#include <glm/glm.hpp>
#include <shader.h>

#define GLM_FORCE_RADIANS
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>

// structures defining the data of both buffers
struct plight {
	glm::vec4 color;
	glm::vec4 position;
	glm::vec4 paddingAndRadius;
};

struct VisibleIndex {
	int index;
};

class ForwardPlusRenderer
{
public:
	ForwardPlusRenderer();
	~ForwardPlusRenderer();

	bool Initialize(unsigned int H, unsigned int W);
	bool SetResolution(unsigned int H, unsigned int W);
	void UpdateLights(std::vector<PointLight*>& m_PointLights);
	void Cleanup();

	void DepthPass(Camera& cam);
	void SetModel(glm::mat4 model);

	void LightCullingPass(Camera& cam);

	Shader light_accumulation;
	Shader debugShader;
	Shader depthdebugShader;
	// X and Y work group dimension variables for compute shader
	GLuint workGroupsX = 0;
	GLuint workGroupsY = 0;
	size_t NUM_LIGHTS = 1024;
	size_t MAX_LIGHTS_PER_TILE = 1024;
	size_t lightCount = 0;

	Shader depthShader;
	Shader lightCullingShader;
	
	glm::ivec2 SCREEN_SIZE;

	// Used for storage buffer objects to hold light data and visible light indicies data
	GLuint lightBuffer = 0;
	GLuint visibleLightIndicesBuffer = 0;
	
	GLuint depthMapFBO, depthMap;

private:
	bool first = true;
};

#endif