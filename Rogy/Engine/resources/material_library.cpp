#include "material_library.h"

MaterialLibrary::MaterialLibrary()
{
}

MaterialLibrary::~MaterialLibrary()
{
}

void MaterialLibrary::Init()
{
	Shader::gNUM_LIGHTS = MAX_LIGHT_COUNT;
	Shader::gNUM_CASCADES = cascadesCount;

	// Load Shaders
	// ------------------------------------------------
	DebugSH			  .loadShader("core/shaders/Debug.rsh");
	SSAOBlur		  .loadShader("core/shaders/PostProc/ssao_blur.rsh");
	shaderSSAO        .loadShader("core/shaders/PostProc/ssao.rsh");
	ShaderGeometryPass.loadShader("core/shaders/PostProc/ssao_geometry.rsh");
	DepthShader		  .loadShader("core/shaders/ShadowMapping/dir_shadow.rsh");
	NonShader		  .loadShader("core/shaders/NonShader.rsh");
	PbrShader		  .loadShader("core/shaders/iblShader.rsh");
	PbrShaderDisp	  .loadShader("core/shaders/iblShader.rsh", "DISPLACEMENT");
	background		  .loadShader("core/shaders/background.rsh");
	PointLightPass	  .loadShader("core/shaders/ShadowMapping/point_shadows_pass.rsh");
	PointDepthShader  .setShader_g("core/shaders/ShadowMapping/point_shadows_depth.vs", 
								   "core/shaders/ShadowMapping/point_shadows_depth.fs",
								   "core/shaders/ShadowMapping/point_shadows_depth.gs");
	BillboardShader.loadShader("core/shaders/Billboard.rsh");
	LM.loadShader("core/shaders/lm.rsh");
	PsShader.loadShader("core\\shaders\\Particle.rsh");
	PsShaderShadow.loadShader("core\\shaders\\ParticleShadow.rsh");
	mgrass.loadShader("core\\shaders\\grass.rsh");
	SkelShader.loadShader("core/shaders/Skeletal.rsh");
	DepthShader_sk.loadShader("core/shaders/ShadowMapping/dir_shadow_sk.rsh");
	mousePickID.loadShader("core\\shaders\\\MousePick\\MPick.rsh");
	PbrShader2.loadShader("core/shaders/iblShader.rsh", "DISPLACEMENT");
	// Prepare shaders
	// ------------------------------------------------
	mgrass.use();
	mgrass.setInt("BBTexture", 0);
	mgrass.setInt("shadowMaps", 1);

	DepthShader.use();
	DepthShader.setInt("alpha", 0);

	PsShader.use();
	PsShader.setInt("myTextureSampler", 0);

	PsShaderShadow.use();
	PsShaderShadow.setInt("myTextureSampler", 0);
	PsShaderShadow.setInt("shadowMaps", 1);

	LM.use();
	LM.setInt("u_lightmap", 0);

	DebugSH.use();
	DebugSH.setInt("screenTexture", 0);
	
	// Point Lights Shadow pass
	PointLightPass.use();
	for (int i = 0; i < 8; i++)
	{
		PbrShader.setBool(("plight[" + std::to_string(i) + "].active").c_str(), false);
		PbrShader.setInt(("plight[" + std::to_string(i) + "].shadowMap").c_str(), i);
	}
	//PointLightPass.setInt("PointdepthMap", 0);

	// SSAO
	SSAOBlur.use();
	SSAOBlur.setInt("ssaoInput", 0);
	ShaderGeometryPass.use();
	ShaderGeometryPass.setInt("invertedNormals", 0);

	shaderSSAO.use();
	shaderSSAO.setInt("gPosition", 5);
	shaderSSAO.setInt("gNormal"  , 6);
	shaderSSAO.setInt("texNoise" , 7);

	// PBR Background
	background.use();
	background.setInt("environmentMap", 0);
	background.setInt("CloudsTex", 1);

	// Billboard shader
	BillboardShader.use();
	BillboardShader.setInt("BBTexture", 0);

	// PBR Shader
	PbrShader.use();
	PbrShader.setInt("env_probe.irradianceMap", TEX_IRRADIANCE_MAP);
	PbrShader.setInt("env_probe.prefilterMap" , TEX_PREFILTER_MAP);

	PbrShader.setInt("material.tex_albedo"   , TEX_ALBEDO);
	PbrShader.setInt("material.tex_metal"    , TEX_METALLIC);
	PbrShader.setInt("material.tex_rough"    , TEX_ROUGHNESS);
	PbrShader.setInt("material.tex_normal"   , TEX_NORMAL);
	PbrShader.setInt("material.tex_emission" , TEX_EMISSION);

	PbrShader.setInt("tex_lightmap" , TEX_AO);

	PbrShader.setInt("shadowMaps[0]", TEX_SHADOWMAP_1);
	PbrShader.setInt("shadowMaps[1]", TEX_SHADOWMAP_2);
	PbrShader.setInt("shadowMaps[2]", TEX_SHADOWMAP_3);

	for (int i = 0; i < 8; i++)
	{
		PbrShader.setInt(("tex_shadows[" + std::to_string(i) + "]").c_str(), TEX_CUBE_SHADOWMAP + i);
	}

	for (int i = 0; i < 8; i++)
	{
		PbrShader.setInt(("texSpot_shadows[" + std::to_string(i) + "]").c_str(), TEX_CUBE_SHADOWMAP + 8 + i);
	}

	PbrShader.SetVec3("material.albedo", glm::vec3(1));
	PbrShader.SetFloat("material.ao", 1.0f);

	for (int i = 0; i < MAX_LIGHT_COUNT; i++)
	{
		PbrShader.setBool(("p_lights[" + std::to_string(i) + "].active").c_str(), false);
	}

	// PBR Shader
	PbrShader2.use();
	PbrShader2.setInt("env_probe.irradianceMap", TEX_IRRADIANCE_MAP);
	PbrShader2.setInt("env_probe.prefilterMap", TEX_PREFILTER_MAP);

	PbrShader2.setInt("material.tex_albedo", TEX_ALBEDO);
	PbrShader2.setInt("material.tex_metal", TEX_METALLIC);
	PbrShader2.setInt("material.tex_rough", TEX_ROUGHNESS);
	PbrShader2.setInt("material.tex_normal", TEX_NORMAL);
	PbrShader2.setInt("material.tex_emission", TEX_EMISSION);

	PbrShader2.setInt("tex_lightmap", TEX_AO);

	PbrShader2.setInt("shadowMaps[0]", TEX_SHADOWMAP_1);
	PbrShader2.setInt("shadowMaps[1]", TEX_SHADOWMAP_2);
	PbrShader2.setInt("shadowMaps[2]", TEX_SHADOWMAP_3);

	for (int i = 0; i < 8; i++)
	{
		PbrShader2.setInt(("tex_shadows[" + std::to_string(i) + "]").c_str(), TEX_CUBE_SHADOWMAP + i);
	}

	for (int i = 0; i < 8; i++)
	{
		PbrShader2.setInt(("texSpot_shadows[" + std::to_string(i) + "]").c_str(), TEX_CUBE_SHADOWMAP + 8 + i);
	}

	PbrShader2.SetVec3("material.albedo", glm::vec3(1));
	PbrShader2.SetFloat("material.ao", 1.0f);

	for (int i = 0; i < MAX_LIGHT_COUNT; i++)
	{
		PbrShader2.setBool(("p_lights[" + std::to_string(i) + "].active").c_str(), false);
	}
	// Displacement Shader
	PbrShaderDisp.use();
	PbrShaderDisp.setInt("env_probe.irradianceMap", TEX_IRRADIANCE_MAP);
	PbrShaderDisp.setInt("env_probe.prefilterMap", TEX_PREFILTER_MAP);

	PbrShaderDisp.setInt("material.tex_albedo", TEX_ALBEDO);
	PbrShaderDisp.setInt("material.tex_metal", TEX_METALLIC);
	PbrShaderDisp.setInt("material.tex_rough", TEX_ROUGHNESS);
	PbrShaderDisp.setInt("material.tex_normal", TEX_NORMAL);

	PbrShaderDisp.setInt("tex_lightmap", TEX_AO);

	PbrShaderDisp.setInt("shadowMaps[0]", TEX_SHADOWMAP_1);
	PbrShaderDisp.setInt("shadowMaps[1]", TEX_SHADOWMAP_2);
	PbrShaderDisp.setInt("shadowMaps[2]", TEX_SHADOWMAP_3);

	for (int i = 0; i < 8; i++)
	{
		PbrShaderDisp.setInt(("tex_shadows[" + std::to_string(i) + "]").c_str(), TEX_CUBE_SHADOWMAP + i);
	}

	for (int i = 0; i < 8; i++)
	{
		PbrShaderDisp.setInt(("texSpot_shadows[" + std::to_string(i) + "]").c_str(), TEX_CUBE_SHADOWMAP + 8 + i);
	}

	PbrShaderDisp.SetVec3("material.albedo", glm::vec3(1));
	PbrShaderDisp.SetFloat("material.ao", 1.0f);

	// SK Shader
	SkelShader.use();
	SkelShader.setInt("irradianceMap", TEX_IRRADIANCE_MAP);
	SkelShader.setInt("prefilterMap", TEX_PREFILTER_MAP);

	SkelShader.setInt("tex_albedo", TEX_ALBEDO);
	SkelShader.setInt("tex_metal", TEX_METALLIC);
	SkelShader.setInt("tex_rough", TEX_ROUGHNESS);
	SkelShader.setInt("tex_normal", TEX_NORMAL);
	SkelShader.setInt("tex_emission", TEX_EMISSION);

	SkelShader.setInt("tex_lightmap", TEX_AO);

	SkelShader.setInt("shadowMaps[0]", TEX_SHADOWMAP_1);
	SkelShader.setInt("shadowMaps[1]", TEX_SHADOWMAP_2);
	SkelShader.setInt("shadowMaps[2]", TEX_SHADOWMAP_3);

	for (int i = 0; i < 8; i++)
	{
		SkelShader.setInt(("tex_shadows[" + std::to_string(i) + "]").c_str(), TEX_CUBE_SHADOWMAP + i);
	}

	for (int i = 0; i < 8; i++)
	{
		SkelShader.setInt(("texSpot_shadows[" + std::to_string(i) + "]").c_str(), TEX_CUBE_SHADOWMAP + 8 + i);
	}

	SkelShader.SetVec3("albedo", glm::vec3(1));
	SkelShader.SetFloat("ao", 1.0f);

	for (int i = 0; i < MAX_LIGHT_COUNT; i++)
	{
		SkelShader.setBool(("p_lights[" + std::to_string(i) + "].active").c_str(), false);
	}
	
	// Create default material
	CreateMaterial("")->isDefault = true;
}

Material* MaterialLibrary::CreateMaterial(const char* mName)
{
	Material* mat_ptr = GetMaterialN(mName);
	if (mat_ptr != nullptr)
	{
		return mat_ptr;
	}
	Material* mat = new Material();
	materials.push_back(mat);

	mat->id = last_used_id;
	mat->name = std::string(mName);
	mat->type = SHADER_PBR;

	last_used_id++;

	return mat;
}

bool MaterialLibrary::RemoveMaterial(int item_id)
{
	for (size_t i = 0; i < materials.size(); i++)
	{
		if (materials[i]->id == item_id)
		{
			return true;
		}
	}
	return false;
}

Material* MaterialLibrary::GetMaterialN(const char* mName)
{
	for (size_t i = 0; i < materials.size(); i++)
	{
		if (materials[i]->name == mName)
		{
			return materials[i];
		}
	}
	return nullptr;
}
Material* MaterialLibrary::GetMaterial(int mID)
{
	for (size_t i = 0; i < materials.size(); i++)
	{
		if (materials[i]->id == mID)
		{
			return materials[i];
		}
	}
	return nullptr;
}

void MaterialLibrary::Clear()
{
	materials.clear();
	materials.shrink_to_fit();
}