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

	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, TEXEL_SIZE, TEXEL_SIZE * SHADOW_MAP_CASCADE_COUNT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER::CSM:: Framebuffer is not complete!";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//std::cout << "INIT CSM.." << SHADOW_MAP_CASCADE_COUNT << "|" << TEXEL_SIZE << "\n";

for (size_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		cascades.emplace_back();
		ShadowCascade* sc = &cascades[cascades.size() - 1];
		
		if (sc == nullptr)
		{
			std::cout << "nullptr in CSM.. weired!\n";
			continue;
		}
		sc->TEXEL_SIZE = TEXEL_SIZE;// / (i + 1);
		
		/*glGenFramebuffers(1, &sc->depthMapFBO);
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
		glReadBuffer(GL_NONE);*/
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
	}
	

	if (SHADOW_MAP_CASCADE_COUNT == 1)
		cascades[0].cascadeSplitFar = Shadow_Distance;
	else if (SHADOW_MAP_CASCADE_COUNT == 2)
	{
		cascades[0].cascadeSplitFar = Shadow_Distance * CascadeSplits[1];
		cascades[1].cascadeSplitFar = Shadow_Distance * CascadeSplits[2];
	}
	else if (SHADOW_MAP_CASCADE_COUNT == 3)
	{
		cascades[0].cascadeSplitFar = Shadow_Distance * CascadeSplits[0];
		cascades[1].cascadeSplitFar = Shadow_Distance * CascadeSplits[1];
		cascades[2].cascadeSplitFar = Shadow_Distance * CascadeSplits[2];
	}

	initalized = true;
}
void ShadowMapper::Clear()
{
	if (initalized)
	{
		glDeleteFramebuffers(1, &depthMapFBO);
		glDeleteTextures(1, &depthMap);
		for (size_t i = 0; i < cascades.size(); i++)
		{
			//glDeleteFramebuffers(1, &cascades[i].depthMapFBO);
			//glDeleteTextures(1, &cascades[i].depthMap);
		}
		cascades.clear();
		cascades.shrink_to_fit();

		glDeleteFramebuffers(1, &depthMapFBO);
		glDeleteTextures(1, &depthMap);
	}
}

void ShadowMapper::Bind(int sc_index)
{
	ShadowCascade* sc = &cascades[sc_index];
	//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glViewport(0, 0, sc->TEXEL_SIZE, sc->TEXEL_SIZE);
	glBindFramebuffer(GL_FRAMEBUFFER, sc->depthMapFBO);
	//glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	//glCullFace(GL_FRONT);
}

void ShadowMapper::Unbind()
{
	//glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
	return (unsigned int)Shadow_Distance;
}


std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
{
	const auto inv = glm::inverse(proj * view);

	std::vector<glm::vec4> frustumCorners;
	for (unsigned int x = 0; x < 2; ++x)
	{
		for (unsigned int y = 0; y < 2; ++y)
		{
			for (unsigned int z = 0; z < 2; ++z)
			{
				const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
				frustumCorners.push_back(pt / pt.w);
			}
		}
	}

	return frustumCorners;
}

glm::mat4 getLightSpaceMatrix(const float nearPlane, const float farPlane, int SCR_WIDTH, int SCR_HEIGHT, Camera* camera, glm::vec3 lightDir)
{
	//const float nearPlane = camera->NearView;
	//const float farPlane = camera->FarView;

	const auto proj = glm::perspective(
		glm::radians(camera->FOV), (float)SCR_WIDTH / (float)SCR_HEIGHT, nearPlane,
		farPlane);
	const auto corners = getFrustumCornersWorldSpace(proj, camera->GetViewMatrix());

	glm::vec3 center = glm::vec3(0, 0, 0);
	for (const auto& v : corners)
	{
		center += glm::vec3(v);
	}
	center /= corners.size();

	const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::min();
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::min();
	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::min();
	for (const auto& v : corners)
	{
		const auto trf = lightView * v;
		minX = std::min(minX, trf.x);
		maxX = std::max(maxX, trf.x);
		minY = std::min(minY, trf.y);
		maxY = std::max(maxY, trf.y);
		minZ = std::min(minZ, trf.z);
		maxZ = std::max(maxZ, trf.z);
	}

	// Tune this parameter according to the scene
	constexpr float zMult = 10.0f;
	if (minZ < 0)
	{
		minZ *= zMult;
	}
	else
	{
		minZ /= zMult;
	}
	if (maxZ < 0)
	{
		maxZ /= zMult;
	}
	else
	{
		maxZ *= zMult;
	}

	const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

	return lightProjection * lightView;
}
void ShadowMapper::CalcOrthoProjs(int SCR_WIDTH, int SCR_HEIGHT, Camera* inCam, DirectionalLight* light)
{
	float lastSplitDist = 0.01f;
	for (size_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
	{
		float splitFar = cascades[i].cascadeSplitFar;
		cascades[i].LightViewProjection = getLightSpaceMatrix(lastSplitDist, splitFar, SCR_WIDTH, SCR_HEIGHT, inCam, light->Direction);
		lastSplitDist = cascades[i].cascadeSplitFar;
	}
}

void ShadowMapper::CalcOrthoProjss(Camera* inCam, DirectionalLight* light)
{
	// Calculate orthographic projection matrix for each cascade
	float lastSplitDist = 0.01f;
	
	for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++) 
	{
		float splitNear = lastSplitDist;
		float splitFar = cascades[i].cascadeSplitFar;

		glm::mat4 proj = glm::perspective(glm::radians(inCam->FOV), inCam->aspectRatio, splitNear, splitFar);

		GLfloat far = -INFINITY;
		GLfloat near = INFINITY;

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
		glm::vec3 lightDir = normalize(-light->Direction);

		// Get frustum center
		glm::vec3 frustumCenter = glm::vec3(0.0f);
		for (uint32_t i = 0; i < 8; i++)
			frustumCenter += frustumCorners[i];
		frustumCenter /= 8.0f;

		//Multiply all the world space frustum corners with the view matrix of the light
		glm::mat4 lightViewMatrix = glm::lookAt((frustumCenter - lightDir), frustumCenter, glm::vec3(0.0f, 0.0f, 1.0f));

		glm::vec3 arr[8];
		for (unsigned int i = 0; i < 8; ++i)
			arr[i] = glm::vec3(lightViewMatrix * glm::vec4(frustumCorners[i], 1.0f));

		glm::vec3 minO = glm::vec3(INFINITY, INFINITY, INFINITY);
		glm::vec3 maxO = glm::vec3(-INFINITY, -INFINITY, -INFINITY);

		for (glm::vec3& vec : arr)
		{
			minO = glm::min(minO, vec);
			maxO = glm::max(maxO, vec);
		}

		far = maxO.z;
		near = minO.z;

		//Get the longest diagonal of the frustum, this along with texel sized increments is used to keep the shadows from shimmering
		//far top right - near bottom left
		glm::vec3 longestDiagonal = frustumCorners[0] - frustumCorners[6];
		GLfloat lengthOfDiagonal = glm::length(longestDiagonal);
		longestDiagonal = glm::vec3(lengthOfDiagonal);

		glm::vec3 borderOffset = (longestDiagonal - (maxO - minO)) * glm::vec3(0.5f, 0.5f, 0.5f);

		borderOffset *= glm::vec3(1.0f, 1.0f, 0.0f);

		maxO += borderOffset;
		minO -= borderOffset;

		GLfloat worldUnitsPerTexel = lengthOfDiagonal / 1024.0f;
		glm::vec3 vWorldUnitsPerTexel = glm::vec3(worldUnitsPerTexel, worldUnitsPerTexel, 0.0f);
		minO /= vWorldUnitsPerTexel;
		minO = glm::floor(minO);
		minO *= vWorldUnitsPerTexel;

		maxO /= vWorldUnitsPerTexel;
		maxO = glm::floor(maxO);
		maxO *= vWorldUnitsPerTexel;

		glm::mat4 lightOrthoMatrix = glm::ortho(minO.x, maxO.x, minO.y, maxO.y, near-100, far+100);
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
	//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 0. create depth cubemap transformation matrices
	// -----------------------------------------------
	float near_plane = 0.1f;
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
	if (shadowMaps[indx] == nullptr)
		return false;

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
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

void SpotShadowMapper::Init()
{
	if (initialized)
	{
		glDeleteFramebuffers(1, &depthMapFBO);
		glDeleteTextures(1, &depthShadow);
	}
	std::cout << "SpotShadowMapper::Init(): " << MaxShadowCount << "\n";
	
	ShadowAtlasCount = glm::sqrt(MaxShadowCount);

	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthShadow);
	glBindTexture(GL_TEXTURE_2D, depthShadow);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, TEXEL_SIZE * ShadowAtlasCount, TEXEL_SIZE * ShadowAtlasCount, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthShadow, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	initialized = true;
}

void SpotShadowMapper::Start()
{
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	if (!ThereAreStaticLights)
	{
		glViewport(0, 0, TEXEL_SIZE * ShadowAtlasCount, TEXEL_SIZE * ShadowAtlasCount);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
}

glm::mat4 SpotShadowMapper::Bind(int indx, glm::vec3 lightPos, glm::vec3 lightDir, float raduis, float angle)
{
	glm::mat4 depthProjectionMatrix = glm::perspective(glm::radians((angle) * 2), 1.0f, 0.1f, raduis);
	glm::mat4 depthViewMatrix = glm::lookAt(lightPos, lightPos + lightDir, glm::vec3(0, 1, 0));
	shadowMaps[indx]->MVP = depthProjectionMatrix * depthViewMatrix;

	glViewport(shadowMaps[indx]->atlas.x * TEXEL_SIZE, shadowMaps[indx]->atlas.y * TEXEL_SIZE, TEXEL_SIZE, TEXEL_SIZE);

	if (ThereAreStaticLights) 
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(shadowMaps[indx]->atlas.x * TEXEL_SIZE, shadowMaps[indx]->atlas.y * TEXEL_SIZE, TEXEL_SIZE, TEXEL_SIZE);
		glClear(GL_DEPTH_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
	}

	return shadowMaps[indx]->MVP;
}


void SpotShadowMapper::ResetShadowResolution(unsigned int newRes)
{
	if (TEXEL_SIZE == newRes) return;
	TEXEL_SIZE = newRes;

	Init();
}


SpotShadowData* SpotShadowMapper::CreateShadowMap()
{
	if (shadowMaps.size() == MaxShadowCount)
		return nullptr;

	SpotShadowData* scm = new SpotShadowData();
	shadowMaps.push_back(scm);
	scm->index = shadowMaps.size();
	scm->index--;

	unsigned int indexCounter = 0;
	bool dobreak = false;
	for (size_t y = 0; y < ShadowAtlasCount; y++)
	{
		for (size_t x= 0; x < ShadowAtlasCount; x++)
		{
			if (indexCounter == scm->index)
			{
				scm->atlas.x = x;
				scm->atlas.y = y;
				dobreak = true;
				break;
			}
			else
				indexCounter++;
		}
		if (dobreak)
			break;
	}

	//std::cout << "Creating Spot Shadow: " << scm->index << " atlas= " << scm->atlas.x << " : " << scm->atlas.y << std::endl;

	return scm;
}
bool SpotShadowMapper::RemoveShadowMap(int& indx)
{
	if (shadowMaps.empty() || indx == -1)
		return false;

	std::cout << "REMOVE SPOT SHADOW MAP " << indx << std::endl;
	//glDeleteFramebuffers(1, &shadowMaps[indx]->depthMapFBO);
	//glDeleteTextures(1, &shadowMaps[indx]->depthShadow);
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
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
