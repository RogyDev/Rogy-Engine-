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
#include "framebuffer_texture.h"

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
	float vignette_radius = 40.0f;
	float vignette_softness = 0.1f;

	// Color Correction
	bool cc_use = false;
	float cc_brightness = 0.0f;
	float cc_contrast = 1.0f;
	float cc_saturation = 0.0f;

	// Full screen blur
	bool blur_use = false;

	// Bloom
	bool bloom_use = false;
	float bloom_threshold = 1.0f;
	
	// Motion Blue
	bool mb_use = false;
	glm::vec2 cam_velocity;
	int fps;

	GLuint mm;

	// framebuffer configuration
	// -------------------------
	FrameBufferTex colorBuffer;
	/*float scr_w, scr_h;
	unsigned int rbo;
	unsigned int framebuffer;
	unsigned int textureColorbuffer;*/

	// Bluring 
	FrameBufferTex verticalBlur;
	FrameBufferTex BluredScreen;

	// Bloom stuff
	FrameBufferTex bloomTex;
	FrameBufferTex bloomBlurTex;


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

	bool Fxaa = false;
	bool sharpen = false;
	float sharpen_amount = 1.f;

	// Tone Mapping
	int ToneMap = 0;

	Shader screenShader;
	Shader blurShader;
	Shader bloomShader;
	Shader bloomBlurShader;
	
	void Init();
	void SetupBuffer(float w, float h);
	void Bind();
	bool Render(GLuint a, float f, float n);
	bool VerticalBlur(FrameBufferTex& fbt);
	bool HorizontalBlur();
	bool Usable();

	void Clear();

private:

};

#endif