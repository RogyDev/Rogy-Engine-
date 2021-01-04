#ifndef POST_P_H
#define POST_P_H

#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include "shader.h"
#include "gBuffer.h"
#include "ssao_effect.h"
#include "../io/FSerializer.h"

class RPostProcessing
{
public:
	RPostProcessing();
	~RPostProcessing();

	// Post Processing Settings
	//--------------------------
	float exposure = 1.0f;

	// VIGNETTE
	bool vignette_use = false;
	float vignette_radius = 10.0f;
	float vignette_softness = 0.1f;

	// Color Correction
	bool cc_use = false;
	float cc_brightness = 0.0f;
	float cc_contrast = 1.0f;
	float cc_saturation = 0.0f;

	// Motion Blue
	bool mb_use = true;
	glm::vec2 cam_velocity;
	int fps;

	// framebuffer configuration
	// -------------------------
	float scr_w, scr_h;

	unsigned int rbo;
	unsigned int framebuffer;
	unsigned int textureColorbuffer;

	// Bloom stuff
	GLuint bloomFBO;
	GLuint bloomBuffer;

	// SSAO
	// ------------------------------
	bool   use_ssao = false;

	// 0 = Very Low ;  1 = Low  ; 2 = High
	unsigned int ssao_quality = 1;

	// configure g-buffer framebuffer
	GeoBuffer gBuffer;

	// create framebuffer to hold SSAO processing stage 
	SSAOEffect ssaoEffect;
	

	bool Setup_PP = true;
	bool Initialized = false;
	bool Use = false;

	Shader screenShader;
	
	void Init();
	void SetupBuffer(float w, float h);
	void Bind();
	void Render();
	void Clear();

	template <class Archive>
	void serialize(Archive & ar)
	{
		ar(string(SER_VERSION));
		ar(vignette_use);
		ar(vignette_softness);
		ar(vignette_radius);

		ar(cc_use);
		ar(cc_brightness);
		ar(cc_saturation);
		ar(cc_contrast);

		ar(use_ssao);
		ar(ssaoEffect.kernelSize);
		ar(ssaoEffect.radius);
		ar(ssaoEffect.ssao_power);
		ar(ssaoEffect.bias);

		ar(mb_use);
		ar(exposure);
	}

private:

};

#endif