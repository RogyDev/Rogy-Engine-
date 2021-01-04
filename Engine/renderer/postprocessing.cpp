#include "postprocessing.h"

RPostProcessing::RPostProcessing()
{
}

RPostProcessing::~RPostProcessing()
{
}

void RPostProcessing::Init()
{
	Setup_PP = false;

	screenShader.loadShader("core/shaders/PostProc/PostProc.rsh");
	screenShader.use();
	screenShader.setInt("screenTexture", 0);
	screenShader.setInt("ssao", 1);
}

void RPostProcessing::Bind()
{
	if (Setup_PP && Use) 
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		//glEnable(GL_DEPTH_TEST);
	}
}

void RPostProcessing::Clear()
{
	if (Initialized)
	{
		// Colors
		glDeleteRenderbuffers(1, &rbo);
		glDeleteFramebuffers(1, &framebuffer);
		glDeleteTextures(1, &textureColorbuffer);
	}
}

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

void RPostProcessing::SetupBuffer(float w, float h)
{
	if (!Setup_PP && Use)
	{
		scr_h = h;
		scr_w = w;

		Clear();

		std::cout << "Init post proccssing with resolution : " << scr_w << ":" << scr_h << std::endl;

		Setup_PP = true;
		Initialized = true;

		// SSAO
		int qlty = 1;  						   // High
		if		(ssao_quality == 1) qlty = 2;  // Low
		else if (ssao_quality == 0) qlty = 4;  // Very Low

		int ssao_scr_w = (int)scr_w / qlty;
		int ssao_scr_h = (int)scr_h / qlty;

		ssaoEffect.Create(ssao_scr_w, ssao_scr_h);

		gBuffer.Create(ssao_scr_w, ssao_scr_h);

		int SCR_WIDTH = (int)scr_w;
		int SCR_HEIGHT = (int)scr_h;

		// COLOR CORRECTION
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		// create a color attachment texture
		glGenTextures(1, &textureColorbuffer);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)scr_w, (int)scr_h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, (int)scr_w, (int)scr_h, 0, GL_RGBA, GL_FLOAT, NULL);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
		
		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)	
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)scr_w, (int)scr_h); 
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); 
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		/*// ping-pong-framebuffer for blurring
		glGenFramebuffers(1, &bloomFBO);
		glGenTextures(1, &bloomBuffer);
		
		glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO);
		glBindTexture(GL_TEXTURE_2D, bloomBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomBuffer, 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;*/
	}
}

void RPostProcessing::Render()
{
	if (Setup_PP && Use) 
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
								  // clear all relevant buffers
		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
		//glClear(GL_COLOR_BUFFER_BIT);

		screenShader.use();

		/*static glm::vec2 Pixels[9] =
		{
			glm::vec2(scr_w*0.50, scr_h*0.50),

			glm::vec2(scr_w*0.25, scr_h*0.50),
			glm::vec2(scr_w*0.75, scr_h*0.50),
			glm::vec2(scr_w*0.50, scr_h*0.25),
			glm::vec2(scr_w*0.50, scr_h*0.75),

			glm::vec2(scr_w*0.25, scr_h*0.25),
			glm::vec2(scr_w*0.25, scr_h*0.75),
			glm::vec2(scr_w*0.75, scr_h*0.25),
			glm::vec2(scr_w*0.75, scr_h*0.75)
		};

		for (int i = 0; i < 9; i++)
		{
			screenShader.SetVec2(("Pixels[" + std::to_string(i) + "]").c_str(), Pixels[i]);
		}*/
		
		screenShader.SetFloat("p_exposure", exposure);

		screenShader.setBool("v_use", vignette_use);
		screenShader.setBool("cc_use", cc_use);
		screenShader.setBool("ssao_use", use_ssao);
		screenShader.setBool("UseMB", mb_use);

		if (vignette_use)
		{
			screenShader.SetFloat("v_radius", vignette_radius);
			screenShader.SetFloat("v_softness", vignette_softness);
		}
		if (cc_use)
		{
			screenShader.SetFloat("brightness", cc_brightness);
			screenShader.SetFloat("contrast", cc_contrast);
			screenShader.SetFloat("saturation", cc_saturation);
		}
		
		if (mb_use)
		{
			screenShader.SetVec2("MBvelocity", cam_velocity);
			screenShader.SetFloat("motionBlurScale", fps / 60.0f);
		}

		screenShader.SetVec2("u_resolution", scr_w, scr_h);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

		if (use_ssao)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, ssaoEffect.ssaoColorBufferBlur);
		}
	}
}