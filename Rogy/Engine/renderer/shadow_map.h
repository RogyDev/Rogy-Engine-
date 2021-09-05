#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <cfloat>
#include <math.h>
#include <iostream>
#include <vector>
#include <glm\glm.hpp>
#include <glm\matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../camera/Camera.h"
#include "lighting.h"
#include "../shading/shader.h"
#include <string>
#include "../camera/frustum.h"

#define MAX_POINT_SHADOWS 8
#define MAX_SPOT_SHADOWS 8

// Cascaded Shadow Mapper
// ----------------------------------------------------------------
struct ShadowCascade
{
	unsigned int TEXEL_SIZE = 1024;
	unsigned int depthMapFBO;
	unsigned int depthMap;
	glm::mat4  LightViewProjection;
	float cascadeSplitFar;
	glm::vec3 frustumCorners[8];
};

class ShadowMapper
{
public:
	ShadowMapper();
	~ShadowMapper();

	unsigned int SHADOW_MAP_CASCADE_COUNT = 2;
	// 1024, 2048 1524
	unsigned int TEXEL_SIZE = 1024;
	float Shadow_Distance = 150;

	std::vector<ShadowCascade> cascades;

	void Init();
	void CalcOrthoProjs(Camera* inCam, DirectionalLight* light);
	void Bind(int sc_index);
	void Unbind();
	void SetCascadesCount(unsigned int count);
	void SetCascadesResolution(unsigned int count);
	void SetShadowDistance(unsigned int dist);
	unsigned int GetShadowDistance();
	void Clear();

	float CascadeSplits[3];
private:


	bool initalized = false;
	bool CascadesSplitsCalc = false;
	//float* cascadeSplits;
};


// Point Light Shadow Mapper
// ----------------------------------------------------------------
struct ShadowCubeMap
{
	unsigned int depthMapFBO;
	unsigned int depthCubemap;
	unsigned int index;
	bool initialized;
};

class PointShadowMapper
{
public:
	PointShadowMapper();
	~PointShadowMapper();

	unsigned int TEXEL_SIZE = 256;
	
	std::vector<ShadowCubeMap*> shadowMaps;

	void CreateShadowMap(ShadowCubeMap &map);
	void RemoveShadowMap(ShadowCubeMap &map);

	void ResetShadowResolution(unsigned int newRes);

	ShadowCubeMap* CreateShadowMap();
	GLuint GetShadowMap(int indx);
	bool RemoveShadowMap(unsigned int& indx);

	void Bind(Shader &depthShader, int indx, glm::vec3 lightPos, float far_plane);
	void Bind(Shader &depthShader, ShadowCubeMap &map, glm::vec3 lightPos, float far_plane);
	void Unbind();

private:

};

// Spot Light Shadow Mapper
// ----------------------------------------------------------------
struct SpotShadowData
{
	unsigned int depthMapFBO;
	unsigned int depthShadow;
	unsigned int index;
	glm::mat4 MVP;
	bool initialized;
};

class SpotShadowMapper
{
public:
	SpotShadowMapper();
	~SpotShadowMapper();

	unsigned int TEXEL_SIZE = 256;

	std::vector<SpotShadowData*> shadowMaps;

	void CreateShadowMap(SpotShadowData &map);
	void RemoveShadowMap(SpotShadowData &map);

	void ResetShadowResolution(unsigned int newRes);

	SpotShadowData* CreateShadowMap();
	GLuint GetShadowMap(int indx);
	bool RemoveShadowMap(unsigned int& indx);

	glm::mat4 Bind(int indx, glm::vec3 lightPos, glm::vec3 lightDir, float raduis, float angle);
	void Unbind();

private:

};

// Point Light Shadows Buffer
// ----------------------------------------------------------------

class PointShadowBuffer
{
public:
	PointShadowBuffer();
	~PointShadowBuffer();

	unsigned int ShadowFBO;
	unsigned int ShadowColorBuffer;
	unsigned int rbo;

	int SCR_W;
	int SCR_H;

	void RefreshSize(int scr_w, int scr_h);
	void Bind();
	void UnBind();
	void Clear();

private:
	bool initialized = false;
};
