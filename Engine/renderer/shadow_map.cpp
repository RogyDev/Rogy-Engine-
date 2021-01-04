#include "shadow_map.h"

ShadowMapper::ShadowMapper()
{
}

ShadowMapper::~ShadowMapper()
{
}

void ShadowMapper::Init()
{
	Clear();

	CascadesSplitsCalc = false;

	for (size_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		cascades.push_back(ShadowCascade());
		ShadowCascade* sc = &cascades[cascades.size() - 1];
		
		sc->TEXEL_SIZE = TEXEL_SIZE;// / (i + 1);

		glGenFramebuffers(1, &sc->depthMapFBO);
		glGenTextures(1, &sc->depthMap);
		glBindTexture(GL_TEXTURE_2D, sc->depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, sc->TEXEL_SIZE, sc->TEXEL_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

		// attach depth texture as FBO's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, sc->depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sc->depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	initalized = true;
}
void ShadowMapper::Clear()
{
	if (initalized)
	{
		for (size_t i = 0; i < cascades.size(); i++)
		{
			glDeleteFramebuffers(1, &cascades[i].depthMapFBO);
			glDeleteTextures(1, &cascades[i].depthMap);
		}
		cascades.clear();
		cascades.shrink_to_fit();
	}
}

void ShadowMapper::Bind(int sc_index)
{
	ShadowCascade* sc = &cascades[sc_index];
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, sc->TEXEL_SIZE, sc->TEXEL_SIZE);
	glBindFramebuffer(GL_FRAMEBUFFER, sc->depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	//glCullFace(GL_FRONT);
}

void ShadowMapper::Unbind()
{
	//glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ShadowMapper::SetCascadesCount(unsigned int count)
{
	SHADOW_MAP_CASCADE_COUNT = count;
	Init();
}

void ShadowMapper::SetCascadesResolution(unsigned int count)
{
	TEXEL_SIZE = count;
	Init();
}

void ShadowMapper::SetShadowDistance(unsigned int dist)
{
	Shadow_Distance = dist;
}
unsigned int ShadowMapper::GetShadowDistance()
{
	return Shadow_Distance;
}

void ShadowMapper::CalcOrthoProjs(Camera* inCam, DirectionalLight* light)
{
	/*if (!CascadesSplitsCalc)
	{
		CascadesSplitsCalc = true;

		
		for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++) {
			cascades[i].cascadeSplitFar = (float)Shadow_Distance * CascadeSplits[i];
			std::cout << "csm : [" << i << "] -> " << CascadeSplits[i] << " | d = " << cascades[i].cascadeSplitFar << "\n";
		}

		cascadeSplits = new float[SHADOW_MAP_CASCADE_COUNT];

		float nearClip = inCam->NearView;
		float farClip = (float)Shadow_Distance;
		float clipRange = farClip - nearClip;

		float minZ = nearClip;
		float maxZ = nearClip + clipRange;

		float range = maxZ - minZ;
		float ratio = maxZ / minZ;

		std::cout << "csm ----------------------------\n";
		// Calculate split depths based on view camera furstum
		// Based on method presentd in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
		for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++) {
			float p = (i + 1) / static_cast<float>(SHADOW_MAP_CASCADE_COUNT);
			float log = minZ * std::pow(ratio, p);
			float uniform = minZ + range * p;
			float d = 0.95f * (log - uniform) + uniform * 1.5f;
			cascadeSplits[i] = (d - nearClip) / clipRange;
			cascades[i].cascadeSplitFar = d;
			std::cout << "csm : [" << i << "] -> " << cascadeSplits[i]  << " | d = " << d << "\n";
		}
	}*/

	if (SHADOW_MAP_CASCADE_COUNT == 1)
		cascades[0].cascadeSplitFar = (float)Shadow_Distance;
	else if (SHADOW_MAP_CASCADE_COUNT == 2)
	{
		cascades[0].cascadeSplitFar = (float)Shadow_Distance * CascadeSplits[1];
		cascades[1].cascadeSplitFar = (float)Shadow_Distance * CascadeSplits[2];
	}
	else if (SHADOW_MAP_CASCADE_COUNT == 3)
	{
		cascades[0].cascadeSplitFar = (float)Shadow_Distance * CascadeSplits[0];
		cascades[1].cascadeSplitFar = (float)Shadow_Distance * CascadeSplits[1];
		cascades[2].cascadeSplitFar = (float)Shadow_Distance * CascadeSplits[2];
	}

	// Calculate orthographic projection matrix for each cascade
	float lastSplitDist = 0.01f;
	for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++) 
	{
		//cascades[i].cascadeSplitFar = (float)Shadow_Distance * CascadeSplits[i];
		glm::mat4 proj = glm::perspective(glm::radians(inCam->FOV), inCam->aspectRatio, lastSplitDist, cascades[i].cascadeSplitFar);

		Frustum fr;
		glm::mat4 invCam = (proj * inCam->GetViewMatrix());
		fr.Update(invCam);
		
		glm::vec3 frustumCorners[8] = {
			fr.m_points[0],
			fr.m_points[1],
			fr.m_points[2],
			fr.m_points[3],
			fr.m_points[4],
			fr.m_points[5],
			fr.m_points[6],
			fr.m_points[7],
		};
		
		/*float splitDist = CascadeSplits[i];
		glm::vec3 frustumCorners[8] = {
			glm::vec3(-1.0f,  1.0f, -1.0f),
			glm::vec3(1.0f,  1.0f, -1.0f),
			glm::vec3(1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f,  1.0f,  1.0f),
			glm::vec3(1.0f,  1.0f,  1.0f),
			glm::vec3(1.0f, -1.0f,  1.0f),
			glm::vec3(-1.0f, -1.0f,  1.0f),
		};

		// Project frustum corners into world space
		glm::mat4 invCam = glm::inverse(proj * inCam->GetViewMatrix());
		for (uint32_t i = 0; i < 8; i++) {
			glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[i], 1.0f);
			frustumCorners[i] = glm::vec3(invCorner / invCorner.w);
		}

		for (uint32_t i = 0; i < 4; i++) {
			glm::vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
			frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
			frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
		}*/
		
		// Get frustum center
		glm::vec3 frustumCenter = glm::vec3(0.0f);
		for (uint32_t i = 0; i < 8; i++) {
			frustumCenter += frustumCorners[i];
		}
		
		frustumCenter /= 8.0f;

		float radius = 0.0f;
		for (uint32_t i = 0; i < 8; i++) {
			float distance = glm::length(frustumCorners[i] - frustumCenter);
			radius = glm::max(radius, distance);
		}
		radius = std::ceil(radius * 16.0f) / 16.0f;

		int qStep = (int)radius;
		frustumCenter.x = std::round(frustumCenter.x / qStep) * qStep;
		frustumCenter.y = std::round(frustumCenter.y / qStep) * qStep;
		frustumCenter.z = std::round(frustumCenter.z / qStep) * qStep;
		
		glm::vec3 maxExtents = glm::vec3(radius);
		glm::vec3 minExtents = -maxExtents;

		glm::vec3 lightDir = normalize(-light->Direction);

		//glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, inCam->transform.up());
		glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));
		//glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, maxExtents.z - minExtents.z);

		glm::vec3 cascade_extents = maxExtents - minExtents;
		//glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, inCam->transform.up());
		glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, minExtents.z, -minExtents.z + cascade_extents.z);
		cascades[i].LightViewProjection = lightOrthoMatrix * lightViewMatrix;
		
		lastSplitDist = cascades[i].cascadeSplitFar;
	}
}

PointShadowMapper::PointShadowMapper()
{
}

PointShadowMapper::~PointShadowMapper()
{
}

void PointShadowMapper::CreateShadowMap(ShadowCubeMap &map)
{
	// configure depth map FBO
	// -----------------------
	glGenFramebuffers(1, &map.depthMapFBO);

	glGenTextures(1, &map.depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, map.depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, TEXEL_SIZE, TEXEL_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, map.depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, map.depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	map.initialized = true;
}

void PointShadowMapper::Bind(Shader &depthShader, ShadowCubeMap &map, glm::vec3 lightPos, float far_plane)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 0. create depth cubemap transformation matrices
	// -----------------------------------------------
	float near_plane = 0.5f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)TEXEL_SIZE / (float)TEXEL_SIZE, near_plane, far_plane);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	// 1. render scene to depth cubemap
	// --------------------------------
	glViewport(0, 0, TEXEL_SIZE, TEXEL_SIZE);
	glBindFramebuffer(GL_FRAMEBUFFER, map.depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	//glCullFace(GL_FRONT);

	depthShader.use();
	for (unsigned int i = 0; i < 6; ++i)
		depthShader.SetMat4(("shadowMatrices[" + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
	depthShader.SetFloat("far_plane", far_plane);
	depthShader.SetVec3("lightPos", lightPos);
	
}

void PointShadowMapper::RemoveShadowMap(ShadowCubeMap &map)
{
	glDeleteFramebuffers(1, &map.depthMapFBO);
	glDeleteTextures  (1 , &map.depthCubemap);
}

void PointShadowMapper::ResetShadowResolution(unsigned int newRes)
{
	if (TEXEL_SIZE == newRes) return;
	TEXEL_SIZE = newRes;

	for (size_t i = 0; i < shadowMaps.size(); i++)
	{
		glDeleteFramebuffers(1, &shadowMaps[i]->depthMapFBO);
		glDeleteTextures(1, &shadowMaps[i]->depthCubemap);
		CreateShadowMap(*shadowMaps[i]);
	}
}

ShadowCubeMap* PointShadowMapper::CreateShadowMap()
{
	if (shadowMaps.size() == MAX_POINT_SHADOWS)
		return nullptr;
	//std::cout << "creating shadow map : " << shadowMaps.size() << std::endl;
	ShadowCubeMap* scm = new ShadowCubeMap();
	scm->index = shadowMaps.size();
	CreateShadowMap(*scm);
	shadowMaps.push_back(scm);
	return scm;
}
GLuint PointShadowMapper::GetShadowMap(int indx)
{
	return shadowMaps[indx]->depthCubemap;
}

bool PointShadowMapper::RemoveShadowMap(unsigned int& indx)
{
	glDeleteFramebuffers(1, &shadowMaps[indx]->depthMapFBO);
	glDeleteTextures   (1, &shadowMaps[indx]->depthCubemap);
	shadowMaps.erase(shadowMaps.begin() + indx);
	shadowMaps.shrink_to_fit();
	indx = -1;

	for (size_t i = 0; i < shadowMaps.size(); i++)
	{
		shadowMaps[i]->index = i;
		//std::cout << "REMOVE SPOT SHADOW MAP AA " << shadowMaps[i]->index << std::endl;
	}

	return true;
}

void PointShadowMapper::Bind(Shader &depthShader, int indx, glm::vec3 lightPos, float far_plane)
{
	Bind(depthShader, *shadowMaps[indx], lightPos, far_plane);
}

void PointShadowMapper::Unbind()
{
	//glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

PointShadowBuffer::PointShadowBuffer()
{
	initialized = false;
}

PointShadowBuffer::~PointShadowBuffer()
{
}

void PointShadowBuffer::RefreshSize(int scr_w, int scr_h)
{
	if (!initialized || SCR_W != scr_w || SCR_H != scr_h)
	{
		SCR_W = scr_w;
		SCR_H = scr_h;
		std::cout << "Creating Shadow Buffer " << std::endl;
		if (initialized)
		{
			Clear();
		}
		initialized = true;

		glGenFramebuffers(1, &ShadowFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, ShadowFBO);

		glGenTextures(1, &ShadowColorBuffer);
		glBindTexture(GL_TEXTURE_2D, ShadowColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)scr_w, (int)scr_h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ShadowColorBuffer, 0);

		/*glGenTextures(1, &ShadowColorBuffer);
		glBindTexture(GL_TEXTURE_2D, ShadowColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_W, SCR_H, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ShadowColorBuffer, 0);
		*/
		// create and attach depth buffer(renderbuffer)
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_W, SCR_H);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void PointShadowBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ShadowFBO);
	glClear(GL_COLOR_BUFFER_BIT);
}

void PointShadowBuffer::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PointShadowBuffer::Clear()
{
	glDeleteFramebuffers(0, &ShadowFBO);
	glDeleteTextures(0, &ShadowColorBuffer);
	glDeleteRenderbuffers(0, &rbo);
}


// ----------------------------------------------------------------------------------
// Spot Shadow managment
// ----------------------------------------------------------------------------------

SpotShadowMapper::SpotShadowMapper()
{
}

SpotShadowMapper::~SpotShadowMapper()
{
}

void SpotShadowMapper::CreateShadowMap(SpotShadowData &map)
{
	//std::cout << "CREATE SPOT SHADOW MAP" << std::endl;
	glGenFramebuffers(1, &map.depthMapFBO);

	glGenTextures(1, &map.depthShadow);
	glBindTexture(GL_TEXTURE_2D, map.depthShadow);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, TEXEL_SIZE, TEXEL_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glBindFramebuffer(GL_FRAMEBUFFER, map.depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, map.depthShadow, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	map.initialized = true;
}

void SpotShadowMapper::ResetShadowResolution(unsigned int newRes)
{
	if (TEXEL_SIZE == newRes) return;
	TEXEL_SIZE = newRes;

	for (size_t i = 0; i < shadowMaps.size(); i++)
	{
		glDeleteFramebuffers(1, &shadowMaps[i]->depthMapFBO);
		glDeleteTextures(1, &shadowMaps[i]->depthShadow);
		CreateShadowMap(*shadowMaps[i]);
	}
}

glm::mat4 SpotShadowMapper::Bind(int indx, glm::vec3 lightPos, glm::vec3 lightDir, float raduis, float angle)
{
	//std::cout << "Binding | " << indx << " | " << shadowMaps.size() << std::endl;
	glm::mat4 depthProjectionMatrix = glm::perspective(glm::radians((angle) * 2), 1.0f, 0.4f, raduis);
	glm::mat4 depthViewMatrix = glm::lookAt(lightPos, lightPos + lightDir, glm::vec3(0, 1, 0));
	shadowMaps[indx]->MVP = depthProjectionMatrix * depthViewMatrix;

	glViewport(0, 0, TEXEL_SIZE, TEXEL_SIZE);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMaps[indx]->depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	return shadowMaps[indx]->MVP;
}

void SpotShadowMapper::RemoveShadowMap(SpotShadowData &map)
{
	glDeleteFramebuffers(1, &map.depthMapFBO);
	glDeleteTextures(1, &map.depthShadow);
}

SpotShadowData* SpotShadowMapper::CreateShadowMap()
{
	if (shadowMaps.size() == MAX_SPOT_SHADOWS)
		return nullptr;
	//std::cout << "creating shadow map : " << shadowMaps.size() << std::endl;
	SpotShadowData* scm = new SpotShadowData();
	scm->index = shadowMaps.size();
	CreateShadowMap(*scm);
	shadowMaps.push_back(scm);
	return scm;
}
GLuint SpotShadowMapper::GetShadowMap(int indx)
{
	return shadowMaps[indx]->depthShadow;
}

bool SpotShadowMapper::RemoveShadowMap(unsigned int& indx)
{
	if (shadowMaps.empty() || indx == -1)
		return false;

	std::cout << "REMOVE SPOT SHADOW MAP " << indx << std::endl;
	glDeleteFramebuffers(1, &shadowMaps[indx]->depthMapFBO);
	glDeleteTextures(1, &shadowMaps[indx]->depthShadow);
	shadowMaps.erase(shadowMaps.begin() + indx);
	shadowMaps.shrink_to_fit();
	indx = -1;

	for (size_t i = 0; i < shadowMaps.size(); i++)
	{
		shadowMaps[i]->index = i;
		//std::cout << "REMOVE SPOT SHADOW MAP AA " << shadowMaps[i]->index << std::endl;
	}

	return true;
}

void SpotShadowMapper::Unbind()
{
	//glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
