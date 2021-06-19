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
	blurShader.loadShader("core/shaders/PostProc/bloomBlur.rsh");

	blurShader.use();
	blurShader.setInt("image", 0);

	screenShader.use();
	screenShader.setInt("screenTexture", 0);
	screenShader.setInt("ssao", 1);
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
	verticalBlur.Clear();
	BluredScreen.Clear();
}

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

void RPostProcessing::SetupBuffer(float w, float h)
{
	if (!Setup_PP && Use)
	{
		Clear();

		std::cout << "Init post proccssing with resolution : " << w << ":" << h << std::endl;

		Setup_PP = true;
		Initialized = true;

		// SSAO
		int qlty = 1;  						   // High
		if		(ssao_quality == 1) qlty = 2;  // Low
		else if (ssao_quality == 0) qlty = 4;  // Very Low
		qlty = 4;
		int ssao_scr_w = (int)w / qlty;
		int ssao_scr_h = (int)h / qlty;

		ssaoEffect.Create(ssao_scr_w, ssao_scr_h);

		gBuffer.Create(ssao_scr_w, ssao_scr_h);

		colorBuffer.Generate((int)w, (int)h, true);

		int b = 10;
		verticalBlur.Generate((int)w/b, (int)h/b, false);
		BluredScreen.Generate((int)w/b, (int)h/b, false);
	}
}

bool RPostProcessing::Render()
{
	if (Setup_PP && Use) 
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
			screenShader.SetVec2(("Pixels[" + std::to_string(i) + "]").c_str(), Pixels[i]);*/
		
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

		screenShader.SetVec2("u_resolution", (float)colorBuffer.scr_w, (float)colorBuffer.scr_h);
		glActiveTexture(GL_TEXTURE0);

		colorBuffer.UseTexture();
			
		//verticalBlur.UseTexture();
		//BluredScreen.UseTexture();

		//glBindTexture(GL_TEXTURE_2D, colorBuffer.textureColorbuffer);

		if (use_ssao)
		{
			glActiveTexture(GL_TEXTURE1);
			//glBindTexture(GL_TEXTURE_2D, ssaoEffect.ssaoColorBufferBlur);
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
