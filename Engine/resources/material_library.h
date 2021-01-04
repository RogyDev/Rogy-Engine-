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
	TEX_SHADOWMAP_1	   = 8,
	TEX_SHADOWMAP_2	   = 9,
	TEX_SHADOWMAP_3    = 10,
	TEX_CUBE_SHADOWMAP = 11,

};
class MaterialLibrary
{
public:
	MaterialLibrary();
	~MaterialLibrary();

	// Shaders
	// ----------------------------------
	Shader PbrShader;
	Shader DepthShader;
	Shader PointDepthShader;
	Shader NonShader;
	Shader background;
	Shader PointLightPass;
	Shader BillboardShader;

	Shader ShaderGeometryPass;
	Shader shaderSSAO;
	Shader SSAOBlur;
	Shader DebugSH;

	// Particle System Shader
	Shader PsShader;
	Shader PsShaderShadow;

	Shader LM;

	Shader mgrass;

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

	int cascadesCount = 3;
private:
	int last_used_id = 0;
};

#endif // ! RMATERIAL_LIBRARY_H