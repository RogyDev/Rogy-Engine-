#ifndef RMATERIAL_LIBRARY_H
#define RMATERIAL_LIBRARY_H

#define MAX_LIGHT_COUNT 47

#include <vector>
#include <shader.h>
#include "../shading/material.h"
#include "texture.h"

enum ShaderTextureType
{
	TEX_IRRADIANCE_MAP = 0,
	TEX_PREFILTER_MAP  = 1,
	TEX_ALBEDO		   = 2,
	TEX_METALLIC	   = 3,
	TEX_ROUGHNESS	   = 4,
	TEX_NORMAL		   = 5,
	TEX_AO             = 6,
	TEX_EMISSION	   = 7,
	TEX_DIR_SHADOWMAPS = 8,
	TEX_SPOT_SHADOWMAPS = 9,
	TEX_IRRADIANCE_PROBES = 10,
	TEX_PREFILTER_PROBES = 11,
	TEX_CUBE_SHADOWMAP = 12,
};

enum ShaderShadowQuality
{
	SQ_LOW = 1,
	SQ_MEDUIM = 2,
	SQ_HIGH = 3,
	SQ_VERYHIGH = 4,
	SQ_ULTRA = 5,
};

class MaterialLibrary
{
public:
	MaterialLibrary();
	~MaterialLibrary();

	// Shaders
	// ----------------------------------
	Shader PbrShader;
	Shader PbrShader2;
	Shader PbrShaderDisp;
	Shader SkelShader;
	Shader DepthShader;
	Shader DepthShader_sk;
	Shader PointDepthShader;
	Shader NonShader;
	Shader background;
	Shader Atmosphere;
	Shader PointLightPass;
	Shader BillboardShader;
	Shader ReflectionShader;

	Shader OutlineObject;
	Shader OutlineScreen;

	Shader ShaderGeometryPass;
	Shader shaderSSAO;
	Shader SSAOBlur;
	Shader DebugSH;

	Shader GridShader;

	// Particle System Shader
	Shader PsShader;
	Shader PsShaderShadow;

	Shader LM;

	Shader mgrass;

	Shader mousePickID;

	Shader Test;

	// Materials
	// -----------------------------------
	std::vector<Material*> materials;

	void Init();

	// ** Create a new material and returns a pointer to it.
	// ** if the material name exists, return existed material without create a new one.
	// ** pass "" empty name to get the default material.
	// try to not modify the default material.
	Material* CreateMaterial(const char* mName);

	bool RemoveMaterial(int item_id);

	// ** Get an existed material created before.
	// ** returns nullptr if not found.
	// ** pass "" empty name to get the default material.
	// try to not modify the default material.
	Material* GetMaterialN(const char* mName);

	Material* GetMaterial(int mID);

	void Clear();
	void ReloadShaders();
	void ReloadMainShaders();

	int cascadesCount = 3;
	ShaderShadowQuality ShadowQuality = ShaderShadowQuality::SQ_MEDUIM;

private:
	int last_used_id = 0;
	void SetShaderMatraciesPos(Shader& a_shader);
};

#endif // ! RMATERIAL_LIBRARY_H