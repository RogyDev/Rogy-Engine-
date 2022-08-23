#include "IBL.h"
// ------------------------------------------------------------------------
IBL::IBL()
{
}
// ------------------------------------------------------------------------
IBL::~IBL()
{
}
// ------------------------------------------------------------------------
void IBL::DeleteCapture(PBRCapture &capture)
{
	glDeleteTextures(1, &capture.Irradiance);
	glDeleteTextures(1, &capture.Prefiltered);
	capture.created = false;
}
// ------------------------------------------------------------------------
void IBL::Init()
{
	// pbr: Load baking shaders (independent from MaterialLibrary)
	// ---------------------------------------------
	if (!shadersLoaded) 
	{
		equirectangularToCubemapShader.loadShader("core/shaders/iblBaking/E2Cubemap.rsh");
		irradianceShader.loadShader("core/shaders/iblBaking/irradiance.rsh");
		prefilterShader.loadShader("core/shaders/iblBaking/prefilter.rsh");
		shadersLoaded = true;
	}
	// pbr: setup capture Projection and Views
	// ---------------------------------------------
	captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

	captureViews[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	captureViews[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	captureViews[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

	// pbr: setup framebuffer
	// ---------------------------------------------
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	InitializeReflectionTexture();
}
// ------------------------------------------------------------------------
void IBL::InitializeReflectionTexture()
{
	if (Probes.created)
	{
		glDeleteTextures(1, &Probes.Irradiance);
		glDeleteTextures(1, &Probes.Prefiltered);
		Probes.created = false;
	}

	// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
	// --------------------------------------------------------------------------------
	glGenTextures(1, &Probes.Irradiance);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, Probes.Irradiance);

	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_RGB16F, 32, 32, 6 * ReflectionProbeLimit + 6 * ReflectionProbeLimit, 0, GL_RGB, GL_FLOAT, nullptr);

	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);

	// pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
	// --------------------------------------------------------------------------------
	glGenTextures(1, &Probes.Prefiltered);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, Probes.Prefiltered);
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minifcation filter to mip_linear 
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_RGB16F, CaptureResolution, CaptureResolution, 6 * ReflectionProbeLimit + 6 * ReflectionProbeLimit, 0, GL_RGB, GL_FLOAT, nullptr);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP_ARRAY);	// generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.

	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);

	Probes.created = true;
}
// ------------------------------------------------------------------------
void IBL::CopyCubemapAtIndex(int indx, GLuint cubemap_id)
{
	/*glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, Probes.Irradiance);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	glViewport(0, 0, 32, 32);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);*/
	
	//glCopyImageSubData(cubemap_id, GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, Probes.Irradiance, GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, 0, 32, 32, 6);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
// ------------------------------------------------------------------------
GLuint IBL::LoadHDR(const char* pth)
{
	// pbr: load the HDR environment map
	// ---------------------------------
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float *data = stbi_loadf(pth, &width, &height, &nrComponents, 0);
	unsigned int hdrTexture;
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load HDR image." << std::endl;
	}

	// pbr: setup cubemap to render to and attach to framebuffer
	// ---------------------------------------------------------
	GLuint envCubemap;
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// pbr: setup capture Projection and Views
	// ---------------------------------------------
	captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

	captureViews[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	captureViews[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	captureViews[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	captureViews[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

	// pbr: convert HDR equirectangular environment map to cubemap equivalent
	// ----------------------------------------------------------------------
	equirectangularToCubemapShader.use();
	equirectangularToCubemapShader.setInt("equirectangularMap", 0);
	equirectangularToCubemapShader.SetMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);

	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		equirectangularToCubemapShader.SetMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_CULL_FACE);
		renderCube();
		glEnable(GL_CULL_FACE);	
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glDeleteTextures(1, &hdrTexture);
	return envCubemap;
}
// ------------------------------------------------------------------------
void IBL::CreateIrradiance(GLuint envMap, GLuint &irradianceMap, bool removeHDR, bool created)
{
	// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
	// --------------------------------------------------------------------------------
	/*if (false && !created) {
		glGenTextures(1, &irradianceMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	} */

	//glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, Probes.Irradiance);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
	// -----------------------------------------------------------------------------
	irradianceShader.use();
	irradianceShader.setInt("environmentMap", 0);
	irradianceShader.SetMat4("projection", captureProjection);
	irradianceShader.setBool("removeHDR", removeHDR);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envMap);

	glViewport(0, 0, 32, 32); 
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	int offset = irradianceMap * 6;
	for (unsigned int i = 0; i < 6; ++i)
	{
		irradianceShader.SetMat4("view", captureViews[i]);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, Probes.Irradiance, 0, (offset + i));

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_CULL_FACE);
		renderCube();
		glEnable(GL_CULL_FACE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
// ------------------------------------------------------------------------
void IBL::CreatePreFilter(GLuint envMap, GLuint &prefilterMap, bool removeHDR, bool created)
{
	// pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
	// --------------------------------------------------------------------------------
	/*if (false && !created) {
		glGenTextures(1, &prefilterMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minifcation filter to mip_linear 
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}*/
	
	// pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
	// ----------------------------------------------------------------------------------------------------
	prefilterShader.use();
	prefilterShader.setInt("environmentMap", 0);
	prefilterShader.SetMat4("projection", captureProjection);
	prefilterShader.setBool("removeHDR", removeHDR);
	prefilterShader.SetFloat("resolution", 128);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envMap);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	int offset = prefilterMap * 6;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader.SetFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader.SetMat4("view", captureViews[i]);
			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
			glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, Probes.Prefiltered, mip, (offset + i));

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glDisable(GL_CULL_FACE);
			renderCube();
			glEnable(GL_CULL_FACE);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
// ------------------------------------------------------------------------
void IBL::CreateLUT(GLuint &brdfLUTTexture)
{
	// pbr: generate a 2D LUT from the BRDF equations used.
	// ----------------------------------------------------
	glGenTextures(1, &brdfLUTTexture);

	// pre-allocate enough memory for the LUT texture.
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	// be sure to set wrapping mode to GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	glViewport(0, 0, 512, 512);
	brdfShader->use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glDisable(GL_CULL_FACE);
	renderQuad();
	glEnable(GL_CULL_FACE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
// ------------------------------------------------------------------------
void IBL::CreateCapture(GLuint envMap, PBRCapture &capture, bool removeHDR)
{
	CreateIrradiance(envMap, capture.Irradiance, removeHDR, capture.created);
	CreatePreFilter(envMap, capture.Prefiltered, removeHDR, capture.created);
	capture.created = true;

	// Configure the viewport to the original framebuffer's screen dimensions
	if (window != nullptr)
	{
		int scrWidth, scrHeight;
		glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
		glViewport(0, 0, scrWidth, scrHeight);
	}
}
// ------------------------------------------------------------------------
void IBL::ScreenRestore(GLFWwindow *wind)
{
	// then before rendering, configure the viewport to the original framebuffer's screen dimensions
	int scrWidth, scrHeight;
	glfwGetFramebufferSize(wind, &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);
}
// ------------------------------------------------------------------------