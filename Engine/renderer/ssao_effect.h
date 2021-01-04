#ifndef R_SSAO_EFFECT
#define R_SSAO_EFFECT

#include "GL\glew.h"
#include "shader.h"
#include "glm\glm.hpp"
#include <iostream>
#include <vector>
#include <random>

class SSAOEffect
{
public:
	SSAOEffect();
	~SSAOEffect();

	// configure g-buffer framebuffer
	// ------------------------------
	Shader *shaderSSAO;
	Shader *SSAOBlur;

	unsigned int ssaoFBO, ssaoBlurFBO;
	unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
	unsigned int noiseTexture;
	std::vector<glm::vec3> ssaoNoise;
	std::vector<glm::vec3> ssaoKernel;

	int SCR_H, SCR_W;

	// Preperties
	float  ssao_power = 1.0f;
	int    kernelSize = 10;
	float  radius     = 3.0f;
	float  bias       = 0.1f;

	void Create(int w, int h);
	void Clear();
	void Bind(glm::mat4 proj);
	void UnBind();
	void Blur();

private:
	bool initilazed = false;
};

#endif // !R_SSAO_EFFECT
