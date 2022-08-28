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
	blurShader.loadShader("core/shaders/PostProc/SSOABlur.rsh");
	bloomShader.loadShader("core/shaders/PostProc/bloom.rsh");
	bloomBlurShader.loadShader("core/shaders/PostProc/bloomBlur.rsh");
	SharpenShader.loadShader("core/shaders/PostProc/Sharpen.rsh");

	bloomShader.use();
	bloomShader.setInt("image", 0);

	bloomBlurShader.use();
	bloomBlurShader.setInt("image", 0);

	blurShader.use();
	blurShader.setInt("image", 0);

	screenShader.use();
	screenShader.setInt("screenTexture", 0);
	screenShader.setInt("HighlightTex", 2);
	screenShader.setInt("ssao", 1);
	screenShader.setInt("bloomBlur", 8);

	SharpenShader.use();
	SharpenShader.setInt("screenTexture", 0);
}

void RPostProcessing::Bind()
{
	if (Setup_PP && Use) 
	{
		colorBuffer.Bind();
	}
}


void RPostProcessing::Clear()
{
	colorBuffer.Clear();
	colorBufferAA.Clear();
	verticalBlur.Clear();
	BluredScreen.Clear();
}

void RPostProcessing::ClearDepth()
{
	/*if (!depth_init || !ScreenSizeChanged) return;

	glDeleteFramebuffers(1, &ScreenDepthFBO);
	glDeleteTextures(1, &ScreenDepth);

	depth_init = false;*/
}

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

void RPostProcessing::SetupBuffer(float w, float h)
{
	/*ClearDepth();
	if (!depth_init)
	{
		std::cout << "Init screen depth texture : " << w << ":" << h << std::endl;
		glGenFramebuffers(1, &ScreenDepthFBO);
		glGenTextures(1, &ScreenDepth);
		glBindTexture(GL_TEXTURE_2D, ScreenDepth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

		// attach depth texture as FBO's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, ScreenDepthFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ScreenDepth, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		depth_init = true;
	}*/

	if (ScreenSizeChanged && Use)
	{
		Clear();

		std::cout << "Init post proccssing with resolution : " << w << ":" << h << std::endl;

		Setup_PP = true;
		Initialized = true;

		/*/ reflection selector
		glGenFramebuffers(1, &RefFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, RefFBO);
		glGenTextures(1, &RefBuffer);
		glBindTexture(GL_TEXTURE_2D, RefBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RefBuffer, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "REF Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

		// SSAO
		int qlty = 1;  						   // High
		if		(ssao_quality == 1) qlty = 2;  // Low
		else if (ssao_quality == 0) qlty = 4;  // Very Low
		qlty = 2;
		int ssao_scr_w = (int)w / qlty;
		int ssao_scr_h = (int)h / qlty;

		ssaoEffect.Create(ssao_scr_w, ssao_scr_h);

		gBuffer.Create(ssao_scr_w, ssao_scr_h);

		colorBuffer.isMain = true;
		colorBuffer.Generate((int)w, (int)h, true);
		colorBufferAA.Generate((int)w, (int)h, false);

		HighlightTex.Generate((int)w, (int)h, false, true);

		int g = 10;
		bloomTex.Generate((int)w/g, (int)h/g, false);
		bloomBlurTex.Generate((int)w/g, (int)h/g, false);

		int r = 20;
		bloomTex2.Generate((int)w / r, (int)h / r, false);
		bloomBlurTex2.Generate((int)w / r, (int)h / r, false);

		int b = 6;
		verticalBlur.Generate((int)w/b, (int)h/b, false);
		BluredScreen.Generate((int)w/b, (int)h/b, false);

		ScreenSizeChanged = false;
	}
}

bool RPostProcessing::ApplyAA()
{
	if (Fxaa && Setup_PP && Use)
	{
		colorBufferAA.Bind();
		glClear(GL_COLOR_BUFFER_BIT);
		SharpenShader.use();
		SharpenShader.SetFloat("sharpen_amount", sharpen_amount);
		SharpenShader.SetVec2("u_resolution", (float)colorBufferAA.scr_w, (float)colorBufferAA.scr_h);
		glActiveTexture(GL_TEXTURE0);
		colorBuffer.UseTexture();
		return true;
	}
	return false;
}

bool RPostProcessing::Render(GLuint a, float f, float n)
{
	if (Setup_PP && Use) 
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		screenShader.use();
		
		screenShader.SetFloat("p_exposure", exposure);

		screenShader.setBool("v_use", vignette_use);
		screenShader.setBool("cc_use", cc_use);
		screenShader.setBool("ssao_use", use_ssao);
		screenShader.setBool("UseMB", mb_use);
		screenShader.setBool("bloom_use", bloom_use);

		screenShader.setBool("use_fxaa", Fxaa);
		screenShader.setBool("sharpen", sharpen);
		
		screenShader.SetFloat("sharpen_amount", sharpen_amount);

		screenShader.setInt("ToneMap", ToneMap);
		
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

		if (bloom_use)
		{
			glActiveTexture(GL_TEXTURE8);
			bloomTex.UseTexture();

			screenShader.setInt("bloomBlur2", 9);
			glActiveTexture(GL_TEXTURE9);
			bloomTex2.UseTexture();
		}

		screenShader.SetVec2("u_resolution", (float)colorBuffer.scr_w, (float)colorBuffer.scr_h);

		glActiveTexture(GL_TEXTURE0);
		if (Fxaa)
			colorBufferAA.UseTexture();
		else
			colorBuffer.UseTexture();

	//	else
			//glBindTexture(GL_TEXTURE_2D, a);
		//bloomTex.UseTexture();

		/*static float sceneExposure = 1.0f;
		glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps every frame
		glm::vec3 luminescence;
		glGetTexImage(GL_TEXTURE_2D, 10, GL_RGB, GL_FLOAT, &luminescence); // Read the value from the lowest mip level
		const float lum = 0.2126f * luminescence.r + 0.7152f * luminescence.g + 0.0722f * luminescence.b; // Calculate a weighted average

		const float adjSpeed = 0.05f;
		sceneExposure = sceneExposure + 0.05f  * ((0.5f / lum * 1.0f) - sceneExposure);//glm::lerp(sceneExposure, 0.5f / lum * 1.0f, 0.5f); // Gradually adjust the exposure
		sceneExposure = glm::clamp(sceneExposure, 0.01f, 20.5f); // Don't let it go over or under a specified min/max range

		screenShader.SetFloat("p_exposure", sceneExposure);
		exposure = sceneExposure;*/

		if (use_ssao)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, ssaoEffect.hBlur.textureColorbuffer);
		}

		return true;
	}
	return false;
}

bool RPostProcessing::VerticalBlur(FrameBufferTex& fbt)
{
	verticalBlur.Bind();
	glClear(GL_COLOR_BUFFER_BIT);
	blurShader.use();
	blurShader.setBool("horizontal", false);
	glActiveTexture(GL_TEXTURE0);
	fbt.UseTexture();
	return true;
}

bool RPostProcessing::HorizontalBlur()
{
	BluredScreen.Bind();
	glClear(GL_COLOR_BUFFER_BIT);
	//blurShader.use();
	blurShader.setBool("horizontal", true);
	//glActiveTexture(GL_TEXTURE0);
	verticalBlur.UseTexture();
	return true;
}

bool RPostProcessing::Usable()
{
	return (Setup_PP && Use);
}
