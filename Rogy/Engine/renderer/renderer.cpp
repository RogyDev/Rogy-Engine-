#include "renderer.h"

#include "cameraComponent.h"
#include "reflection_probe.h"
#include "GrassComponent.h"

IMPL_COMPONENT(ReflectionProbe)
IMPL_COMPONENT(CameraComponent)
IMPL_COMPONENT(GrassComponent)

#define LOG_INFO std::cout << "RENDERER : " <<
#define END_LOG << std::endl
// ------------------------------------------------------------------------
Renderer::Renderer()
{
}
// ------------------------------------------------------------------------
Renderer::~Renderer()
{
}
// ------------------------------------------------------------------------
void Renderer::Clear()
{
	materials.Clear();
	glDeleteFramebuffers(1, &m_FramebufferCubemap);
	glDeleteRenderbuffers(1, &m_CubemapDepthRBO);

	RemoveAllLights();
}
// ------------------------------------------------------------------------
void Renderer::RemoveAllLights()
{
	for (size_t i = 0; i < m_PointLights.size(); i++)
	{
		RemovePointLight(m_PointLights[i]->light_id);
	}
	for (size_t i = 0; i < m_SpotLights.size(); i++)
	{
		RemoveSpotLight(m_SpotLights[i]->light_id);
	}
	RemoveDirectionalLight();

	for (size_t i = 0; i < m_ReflectionProbes.size(); i++)
	{
		RemoveReflectionProbe(m_ReflectionProbes[i]->probe_id);
	}
}
// ------------------------------------------------------------------------
void useShader(Shader& sh, Camera& cam)
{
	sh.use();
	sh.SetMat4("projection", cam.GetProjectionMatrix());
	sh.SetMat4("view", cam.GetViewMatrix());
	sh.SetVec3("CamPos", cam.transform.Position);
}
// ------------------------------------------------------------------------
void usePBRCapture(PBRCapture& capture)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, capture.Irradiance);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, capture.Prefiltered);
}
// ------------------------------------------------------------------------
void Renderer::Initialize(int weight, int height, GLFWwindow* wind, ResourcesManager* resM)
{
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glGenFramebuffers(1, &m_FramebufferCubemap);
	glGenRenderbuffers(1, &m_CubemapDepthRBO);

	// Initialize Material Library; Load Shaders
	materials.cascadesCount = m_ShadowMapper.SHADOW_MAP_CASCADE_COUNT;
	materials.Init();

	// Set ResManager
	resManager = resM;
	window = wind;

	OnViewportResize(0, 0, weight, height);

	MainCam.Initia();
	MainCam.transform.Position = glm::vec3(0, 0, 3);
	MainCam.aspectRatio = (float)weight / (float)height;
	MainCam.ComputeMatrices();

	// Initialize PBR Class
	ibl.Init();
	ibl.window = window;
	//SetEnv_SkyCapture("core\\textures\\bg.hdr");
	SetEnv_SkyCapture("core\\textures\\background.jpg");
	//SetEnv_SkyCapture("res/DaylightAmbientCubemap.HDR");
	//SetEnv_SkyCapture("res/newport_loft.hdr");
	//SetEnv_SkyCapture("res/hdr_bg.jpg");

	// Initialize Post Processor
	postProc.Init();
	postProc.ssaoEffect.shaderSSAO = &materials.shaderSSAO;
	postProc.ssaoEffect.SSAOBlur = &materials.SSAOBlur;

	// Initialize Shadow Mapper
	m_ShadowMapper.SetShadowDistance(m_ShadowMapper.Shadow_Distance);
	m_ShadowMapper.SetCascadesCount(m_ShadowMapper.SHADOW_MAP_CASCADE_COUNT);

	m_cache.pbrShader = &materials.PbrShader;

	//fplus_renderer.Initialize(weight, height);
}
// ------------------------------------------------------------------------
void Renderer::SetEnv_SkyCapture(std::string path)
{
	if (SkyPath == path)
		return;

	SkyPath = path;
	ibl.DeleteCapture(m_SkyCapture);
	glDeleteTextures(1, &envCubemap);
	ibl.LoadHDR(SkyPath.c_str(), envCubemap);
	ibl.LoadHDR("core\\textures\\clouds.jpg", skyClouds);
	ibl.CreateCapture(envCubemap, m_SkyCapture);
}
// ------------------------------------------------------------------------
void Renderer::OnViewportResize(int pos, int top, int weight, int height)
{
	if (SCR_weight != weight || SCR_height != height || left_scr_pos != pos)
	{
		SCR_height = height;
		SCR_weight = weight;
		left_scr_pos = pos;

		postProc.Setup_PP = false;
		MainCam.aspectRatio = (float)SCR_weight / (float)SCR_height;
		entitySelectionBuffer.Generate(weight, height, true, false);
		glViewport(left_scr_pos, top, SCR_weight, SCR_height);
		//fplus_renderer.SetResolution(height, weight);
	}
}
// ------------------------------------------------------------------------
void Renderer::RenderMesh(Mesh* mesh, Material* material, glm::mat4 transform)
{
	useShader(materials.PbrShader, MainCam);

	usePBRCapture(m_SkyCapture);

	materials.PbrShader.SetMat4("model", transform);
	material->setShader(&materials.PbrShader);
	mesh->Draw();
}
// ------------------------------------------------------------------------
void Renderer::PushRender(Mesh* mesh, Material* material, glm::mat4 transform, BBox box, bool cast_shadows, glm::vec3 pos, bool is_static, std::string lmPath, unsigned int entID)
{
	m_RenderBuffer.Push(mesh, material, transform, box, cast_shadows, pos, is_static, lmPath, entID);
}
void Renderer::PushCutoutRender(Mesh* mesh, Material* material, glm::mat4 transform, BBox box, bool cast_shadows, glm::vec3 pos, bool is_static, std::string lmPath, unsigned int entID)
{
	m_RenderBuffer.PushCutout(mesh, material, transform, box, cast_shadows, pos, is_static, lmPath, entID);
}
// ------------------------------------------------------------------------
BillboardComponent* Renderer::CreateBillboard(EnttID ent)
{
	for (size_t i = 0; i < r_billboards.size(); i++)
	{
		if (r_billboards[i]->entid == ent)
			return nullptr;
	}

	BillboardComponent* comp = new BillboardComponent();
	comp->entid = ent;
	r_billboards.push_back(comp);
	return comp;
}
// ------------------------------------------------------------------------
bool Renderer::RemoveBillboard(EnttID ent)
{
	for (size_t i = 0; i < r_billboards.size(); i++)
	{
		if (r_billboards[i]->entid == ent)
		{
			BillboardComponent* comp = r_billboards[i];
			r_billboards.erase(r_billboards.begin() + i);
			delete comp;
			return true;
		}
	}
	return false;
}
void Renderer::AddBillbroadInfo(glm::vec3 pos, glm::vec2 size, const char* texture_path, bool depth_test, glm::vec3 color, bool tex_mask)
{
	bool groupe_found = false;
	for (size_t i = 0; i < m_Billboards.size(); i++)
	{
		if (m_Billboards[i]->texture != nullptr && m_Billboards[i]->texture->getTexName() == texture_path)
		{
			groupe_found = true;
			m_Billboards[i]->positions.push_back(ABillboardState(pos, size, depth_test, color, tex_mask));
			return;
		}
	}
	if (!groupe_found)
	{
		BillboardInfo* bb = new BillboardInfo();
		bb->texture = resManager->CreateTexture("bbtt", texture_path, true);
		bb->positions.push_back(ABillboardState(pos, size, depth_test, color, tex_mask));
		m_Billboards.push_back(bb);
	}
}
// ------------------------------------------------------------------------
void Renderer::RenderBillboards(Camera& cam)
{
	if (m_Billboards.empty()) return;

	glm::mat4 proj = cam.GetProjectionMatrix();
	glm::mat4 view = cam.GetViewMatrix();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	materials.BillboardShader.use();
	materials.BillboardShader.SetMat4("VP", proj * view);
	materials.BillboardShader.SetVec3("CameraUp_worldspace", cam.transform.up());
	materials.BillboardShader.SetVec3("CameraRight_worldspace", cam.transform.right());

	bool depth_disabled = false;
	for (size_t i = 0; i < m_Billboards.size(); i++)
	{
		if (m_Billboards[i]->texture == nullptr)
			continue;

		glActiveTexture(GL_TEXTURE0);
		m_Billboards[i]->texture->useTexture();
		for (size_t j = 0; j < m_Billboards[i]->positions.size(); j++)
		{
			if (!m_Billboards[i]->positions[j].depth_test && !depth_disabled)
			{
				glDisable(GL_DEPTH_TEST);
				depth_disabled = true;
			}
			else if (m_Billboards[i]->positions[j].depth_test && depth_disabled)
			{
				glEnable(GL_DEPTH_TEST);
				depth_disabled = false;
			}
			materials.BillboardShader.SetVec2("BillboardSize", m_Billboards[i]->positions[j].size);
			materials.BillboardShader.SetVec3("BillboardPos", m_Billboards[i]->positions[j].pos);
			materials.BillboardShader.SetVec3("Tex_color", m_Billboards[i]->positions[j].color);
			materials.BillboardShader.setBool("use_tex_as_mask", m_Billboards[i]->positions[j].use_tex_as_mask);
			renderQuad();
		}
	}

	for (size_t i = 0; i < m_Billboards.size(); i++)
	{
		m_Billboards[i]->positions.clear();
	}
	if (depth_disabled)
		glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	m_Billboards.clear();
}
// ------------------------------------------------------------------------
SpotLight* Renderer::CreateSpotLight(EnttID ent_id)
{
	//cout << "creating Point Light." << endl;

	SpotLight* pl = new SpotLight();
	pl->light_id = ent_id;
	//last_used_id++;
	m_SpotLights.push_back(pl);

	return pl;
}
// ------------------------------------------------------------------------
bool Renderer::RemoveSpotLight(EnttID ent_id)
{
	for (size_t i = 0; i < m_SpotLights.size(); i++)
	{
		if (m_SpotLights[i]->light_id == ent_id)
		{
			if (m_SpotLights[i]->CastShadows == true && m_SpotLights[i]->shadow_index != -1)
			{
				m_SpotShadowMapper.RemoveShadowMap(m_SpotLights[i]->shadow_index);
				m_SpotLights[i]->shadow_index = -1;
				ReIndexSpotLightsShadowMaps();
			}

			SpotLight* sl = m_SpotLights[i];
			m_SpotLights.erase(m_SpotLights.begin() + i);
			delete sl;

			return true;
		}
	}
	return false;
}
// ------------------------------------------------------------------------
SpotLight* Renderer::GetSpotLight(EnttID ent_id)
{
	for (size_t i = 0; i < m_SpotLights.size(); i++)
	{
		if (m_SpotLights[i]->light_id == ent_id)
		{
			return m_SpotLights[i];
		}
	}
	return nullptr;
}
// ------------------------------------------------------------------------
PointLight* Renderer::CreatePointLight(EnttID ent_id)
{
	PointLight* pl = new PointLight();
	pl->light_id = ent_id;
	//last_used_id++;
	m_PointLights.push_back(pl);

	return pl;
}
// ------------------------------------------------------------------------
bool Renderer::RemovePointLight(EnttID ent_id)
{
	for (size_t i = 0; i < m_PointLights.size(); i++)
	{
		if (m_PointLights[i]->light_id == ent_id)
		{
			if (m_PointLights[i]->CastShadows == true && m_PointLights[i]->shadow_index != -1)
			{
				m_PointShadowMapper.RemoveShadowMap(m_PointLights[i]->shadow_index);
				ReIndexPointLightsShadowMaps();
			}
			PointLight* pl = m_PointLights[i];
			m_PointLights.erase(m_PointLights.begin() + i);
			delete pl;
		
			return true;
		}
	}
	return false;
}
// ------------------------------------------------------------------------
PointLight* Renderer::GetPointLight(EnttID ent_id)
{
	for (size_t i = 0; i < m_PointLights.size(); i++)
	{
		if (m_PointLights[i]->light_id == ent_id)
		{
			return m_PointLights[i];
		}
	}
	return nullptr;
}
// ------------------------------------------------------------------------
DirectionalLight* Renderer::CreateDirectionalLight()
{
	if (m_DirectionalLight == nullptr)
	{
		m_DirectionalLight = new DirectionalLight();
		m_DirectionalLight->Active = true;
		materials.PbrShader.use();
		materials.PbrShader.SetFloat("dirLight.use", true);
		return m_DirectionalLight;
	}
	else
	{
		std::cout << "DIR LIGHT FIAIAIAIA\n";
	}
	return m_DirectionalLight;
}
// ------------------------------------------------------------------------
bool Renderer::RemoveDirectionalLight()
{
	if (m_DirectionalLight != nullptr)
	{
		delete m_DirectionalLight;
		m_DirectionalLight = nullptr;
		materials.PbrShader.use();
		materials.PbrShader.SetFloat("dirLight.use", false);
		return true;
	}
	return false;
}
// ------------------------------------------------------------------------
void Renderer::RenderParticals(Camera& cam, float dt)
{
	std::vector<ParticleSystem*> particals = mParticals.GetComponents();
	for (size_t i = 0; i < particals.size(); i++)
	{
		ParticleSystem* psc = particals[i];
		if (psc->removed) {
			psc->Clear();
			mParticals.RemoveComponent(psc->entid);
			continue;
		}

		psc->Update(dt, cam.transform.Position);

		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		if (psc->UseTexTransparent)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		else
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		if (psc->ReceiveShadows)
		{
			materials.PsShaderShadow.use();
			glActiveTexture(GL_TEXTURE0);
			if (psc->mTexture)	psc->mTexture->useTexture();

			if (psc->ReceiveShadows && m_DirectionalLight != nullptr && m_DirectionalLight->CastShadows) {
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, m_ShadowMapper.cascades[0].depthMap);
				materials.PsShaderShadow.SetMat4("gLightWVP", m_ShadowMapper.cascades[0].LightViewProjection);
				materials.PsShaderShadow.SetVec3("sunColor", m_DirectionalLight->Color);
			}
			materials.PsShaderShadow.SetMat4("VP", MainCam.GetProjectionMatrix() * MainCam.GetViewMatrix());
			materials.PsShaderShadow.SetVec3("CameraRight_worldspace", MainCam.transform.right());
			materials.PsShaderShadow.SetVec3("CameraUp_worldspace", MainCam.transform.up());
			materials.PsShaderShadow.SetVec2("animSheetSize", psc->AnimSheetSize);
			materials.PsShaderShadow.setBool("use_anim", psc->anim_use);
		}
		else
		{
			materials.PsShader.use();
			glActiveTexture(GL_TEXTURE0);
			if (psc->mTexture)	psc->mTexture->useTexture();
			materials.PsShader.SetMat4("VP", MainCam.GetProjectionMatrix() * MainCam.GetViewMatrix());
			materials.PsShader.SetVec3("CameraRight_worldspace", MainCam.transform.right());
			materials.PsShader.SetVec3("CameraUp_worldspace", MainCam.transform.up());
			materials.PsShader.SetVec2("animSheetSize", psc->AnimSheetSize);
			materials.PsShader.setBool("use_anim", psc->anim_use);
		}

		glBindVertexArray(psc->quadVAO);
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, psc->ParticlesCount);
		glBindVertexArray(0);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}
}
// ------------------------------------------------------------------------
void Renderer::UpdateGameCamera()
{
	if (!use_GameView)
		MainCam.Reset_ini();

	if (use_GameView && !m_cameras.Empty())
	{
		std::vector<CameraComponent*> cams = m_cameras.GetComponents();
		for (size_t i = 0; i < cams.size(); i++)
		{
			if (cams[i]->removed) {
				m_cameras.RemoveComponent(cams[i]->entid);
				continue;
			}
			if (cams[i]->Primary)
			{
				MainCam.Save_ini();
				MainCam.transform.SetPosition(cams[i]->position);
				MainCam.transform.UpVec = cams[i]->up;
				MainCam.transform.RightVec = cams[i]->right;
				MainCam.transform.DirVec = cams[i]->direction;
				MainCam.FOV = cams[i]->FOV;
				MainCam.NearView = cams[i]->NearView;
				MainCam.FarView = cams[i]->FarView;
				break;
			}
		}
	}
}
// ------------------------------------------------------------------------
void Renderer::UpdateCamera()
{
	// Update Camera
	// -------------------------------------------
	MainCam.ComputeMatrices();
}
// ------------------------------------------------------------------------
void Renderer::RenderGrass(Camera & cam, float dt)
{
	mGrass.ClearRemovedComponents();
	std::vector<GrassComponent*> allGrass = mGrass.GetComponents();
	if (!allGrass.empty())
		glDisable(GL_CULL_FACE);
	for (size_t i = 0; i < allGrass.size(); i++)
	{
		GrassComponent* grass = allGrass[i];
		if (grass->mTexture == nullptr) continue;

		materials.mgrass.use();
		materials.mgrass.SetVec3("CameraRight_worldspace", cam.transform.right());
		materials.mgrass.SetMat4("VP", cam.GetProjectionMatrix() * cam.GetViewMatrix());
		materials.mgrass.SetFloat("alpaValue", 0.1f);
		materials.mgrass.SetVec2("BillboardSize", grass->size);
		glActiveTexture(GL_TEXTURE0);
		grass->mTexture->useTexture();

		if (m_DirectionalLight != nullptr && m_DirectionalLight->CastShadows) {
			materials.mgrass.setBool("use_shadows", true);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_ShadowMapper.cascades[0].depthMap);
			materials.mgrass.SetMat4("gLightWVP", m_ShadowMapper.cascades[0].LightViewProjection);
		}
		else
			materials.mgrass.setBool("use_shadows", false);

		for (size_t i = 0; i < grass->mGrass.size(); i++)
		{
			if (glm::distance(grass->mGrass[i], cam.transform.Position) > grass->Distance) continue;

			materials.mgrass.SetVec3("BillboardPos", grass->mGrass[i]);
			renderQuad();
		}
	}
	if (!allGrass.empty())
		glEnable(GL_CULL_FACE);
}
// ------------------------------------------------------------------------
void Renderer::RenderFrame(float dt)
{
	delTime = dt;
	// Handle the remove Notification
	m_renderers.ClearRemovedComponents();

	for (size_t i = 0; i < r_billboards.size(); i++)
	{
		if (r_billboards[i]->removed)
			RemoveBillboard(r_billboards[i]->entid);
	}

	if (skyTexChange)
	{
		skyTexChange = false;
		SetEnv_SkyCapture(new_skyPath);
	}

	// Bake Lighting if requested last frame.
	// -------------------------------------------
	if (BakeLighting)
	{
		BakeLighting = false;
		bakingSucceed = BakeSceneLightmaps();
	}
	MainCam.ComputeMatrices();
	// Update reflection probes
	// -------------------------------------------
	UpdateReflectionProbes();

	// Render Scene Meshes
	// -------------------------------------------
	RenderScene(MainCam);

	// Render Grass
	// -------------------------------------------
	RenderGrass(MainCam, dt);

	// Render Billboards
	// -------------------------------------------
	RenderBillboards(MainCam);

	// Render Particles
	// -------------------------------------------
	RenderParticals(MainCam, dt);
}
// ------------------------------------------------------------------------
void BindTexture(GLuint tex_id, GLuint tex, bool cubmap = false)
{
	glActiveTexture(GL_TEXTURE0 + tex_id);
	if (cubmap)
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
	else
		glBindTexture(GL_TEXTURE_2D, tex);
}
// ------------------------------------------------------------------------
void BindTexture(GLuint tex_id)
{
	glActiveTexture(GL_TEXTURE0 + tex_id);
}
// ------------------------------------------------------------------------
bool Check2BallsIntersect(glm::vec3 fpos, float frad, glm::vec3 spos, float srad)
{
	float dist = glm::distance(fpos, spos);
	dist -= (frad + srad);

	if (dist > 0)
		return true;

	return false;
}
// ------------------------------------------------------------------------
// Cheating with the renderer to fix a bug :D (atleast for now, the future is better :)
void Renderer::ReIndexPointLightsShadowMaps()
{
	size_t idddx = 0;
	for (size_t j = 0; j < m_PointLights.size(); j++)
	{
		if (m_PointLights[j]->CastShadows == true && m_PointLights[j]->shadow_index != -1)
		{
			m_PointLights[j]->shadow_index = m_PointShadowMapper.shadowMaps[idddx]->index;
			idddx++;
			if (m_PointShadowMapper.shadowMaps.size() < (idddx + 1))
				break;
		}
	}
}
void Renderer::ReIndexSpotLightsShadowMaps()
{
	size_t idddx = 0;
	for (size_t j = 0; j < m_SpotLights.size(); j++)
	{
		if (m_SpotLights[j]->CastShadows == true && m_SpotLights[j]->shadow_index != -1)
		{
			m_SpotLights[j]->shadow_index = m_SpotShadowMapper.shadowMaps[idddx]->index;
			idddx++;
			if (m_SpotShadowMapper.shadowMaps.size() < (idddx + 1))
				break;
		}
	}
}
// ------------------------------------------------------------------------
void Renderer::RenderDisplacements()
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (mDisplacements.Empty()) return;

	glm::mat4 proj = MainCam.GetProjectionMatrix();
	glm::mat4 view = MainCam.GetViewMatrix();
	glm::vec3 viewPos = MainCam.transform.Position;

	Shader* pbrShader = &materials.PbrShader2;
	pbrShader->use();
	pbrShader->SetMat4("projection", proj);
	pbrShader->SetMat4("view", view);
	pbrShader->SetVec3("CamPos", viewPos);
	pbrShader->SetFloat("ao", 1.0f);

	// Send Directional Light informations to shader
	// -----------------------------------------------------------------
	if (m_DirectionalLight != nullptr)
	{
		if (m_DirectionalLight->Active)
		{
			pbrShader->setBool("dirLight.cast_shadows", m_DirectionalLight->CastShadows);
			if (m_DirectionalLight->CastShadows)
			{
				for (size_t i = 0; i < m_ShadowMapper.cascades.size(); i++)
				{
					pbrShader->SetMat4(("gLightWVP[" + std::to_string(i) + "]").c_str(), m_ShadowMapper.cascades[i].LightViewProjection);
					pbrShader->SetFloat(("CascadeEndClipSpace[" + std::to_string(i) + "]").c_str(), m_ShadowMapper.cascades[i].cascadeSplitFar);
				}
			}
			pbrShader->SetVec3("dirLight.direction", m_DirectionalLight->Direction);
			pbrShader->SetFloat("dirLight.intensity", m_DirectionalLight->Intensity);
			pbrShader->SetVec3("dirLight.color", m_DirectionalLight->Color);
			pbrShader->setBool("dirLight.soft_shadows", m_DirectionalLight->Soft);
			pbrShader->SetFloat("dirLight.Bias", m_DirectionalLight->Bias);
		}
		pbrShader->SetFloat("dirLight.use", m_DirectionalLight->Active);
	}
	int visibe_lights_index = 0;

	// Send Point Lights informations to shader
	// -----------------------------------------------------------------
	/*for (unsigned int i = 0; i < MAX_LIGHT_COUNT; i++)
	{
		if (m_PointLights.empty())
			break;
		if ((m_PointLights.size() - 1) >= i)
		{
			if (m_PointLights[i]->removed)
			{
				RemovePointLight(m_PointLights[i]->light_id);
				continue;
			}
			if (m_PointLights[i]->isActive())
			{
				m_PointLights[i]->inFrustum = frustum.sphereInFrustum(m_PointLights[i]->Position, m_PointLights[i]->Raduis);

				if (m_PointLights[i]->inFrustum) {

					pbrShader->setInt(("visible_pLights[" + std::to_string(visibe_lights_index) + "]").c_str(), i);
					visibe_lights_index++;

					pbrShader->SetVec4(("p_lights[" + std::to_string(i) + "].position").c_str(), glm::vec4(m_PointLights[i]->Position, m_PointLights[i]->Raduis));
					pbrShader->SetVec4(("p_lights[" + std::to_string(i) + "].color").c_str(), glm::vec4(m_PointLights[i]->Color, m_PointLights[i]->Intensity));
					pbrShader->SetFloat(("p_lights[" + std::to_string(i) + "].cast_shadows").c_str(), m_PointLights[i]->CastShadows);

					if (m_PointLights[i]->CastShadows) {
						pbrShader->SetFloat(("p_lights[" + std::to_string(i) + "].Bias").c_str(), m_PointLights[i]->Bias);
						pbrShader->setInt(("p_lights[" + std::to_string(i) + "].shadow_index").c_str(), m_PointLights[i]->shadow_index);
					}
				}
			}
		}
		else
			break;
	}

	if (visibe_lights_index < MAX_LIGHT_COUNT)
		pbrShader->setInt(("visible_pLights[" + std::to_string(visibe_lights_index) + "]").c_str(), -1);

	visibe_lights_index = 0;

	// Send Spot Lights informations to shader
	// -----------------------------------------------------------------
	for (unsigned int i = 0; i < MAX_LIGHT_COUNT; i++)
	{
		if (m_SpotLights.empty())
			break;
		if ((m_SpotLights.size() - 1) >= i)
		{
			if (m_SpotLights[i]->removed)
			{
				RemoveSpotLight(m_SpotLights[i]->light_id);
				continue;
			}
			if (m_SpotLights[i]->isActive())
			{
				m_SpotLights[i]->inFrustum = frustum.sphereInFrustum(m_SpotLights[i]->Position, m_SpotLights[i]->Raduis);
				if (m_SpotLights[i]->inFrustum) {

					pbrShader->setInt(("visible_sLights[" + std::to_string(visibe_lights_index) + "]").c_str(), i);
					visibe_lights_index++;

					pbrShader->SetVec4(("sp_lights[" + std::to_string(i) + "].direction").c_str(), glm::vec4(m_SpotLights[i]->Direction, m_SpotLights[i]->CutOff));
					pbrShader->SetVec4(("sp_lights[" + std::to_string(i) + "].position").c_str(), glm::vec4(m_SpotLights[i]->Position, m_SpotLights[i]->Raduis));
					pbrShader->SetVec4(("sp_lights[" + std::to_string(i) + "].color").c_str(), glm::vec4(m_SpotLights[i]->Color, m_SpotLights[i]->Intensity));
					pbrShader->SetFloat(("sp_lights[" + std::to_string(i) + "].outerCutOff").c_str(), glm::cos(glm::radians(m_SpotLights[i]->CutOff + m_SpotLights[i]->OuterCutOff)));

					pbrShader->SetFloat(("sp_lights[" + std::to_string(i) + "].cast_shadows").c_str(), m_SpotLights[i]->CastShadows);
					pbrShader->SetFloat(("sp_lights[" + std::to_string(i) + "].Bias").c_str(), m_SpotLights[i]->Bias);
					pbrShader->setInt(("sp_lights[" + std::to_string(i) + "].shadow_index").c_str(), m_SpotLights[i]->shadow_index);
				}
			}
		}
		else
			break;
	}
	if (visibe_lights_index < MAX_LIGHT_COUNT)
		pbrShader->setInt(("visible_sLights[" + std::to_string(visibe_lights_index) + "]").c_str(), -1);*/

	// Set Fog config
	// -----------------------------------------------------------------
	pbrShader->setBool("Fog.use", usefog);
	if (usefog)
	{
		pbrShader->SetVec3("Fog.color", fogColor);
		pbrShader->SetFloat("Fog.far", fogFar);
		pbrShader->SetFloat("Fog.near", fogNear);
	}
	// Send PBR environments maps
	// -----------------------------------------------------------------
	m_cache.BindTexture(TEX_IRRADIANCE_MAP, m_SkyCapture.Irradiance);
	m_cache.BindTexture(TEX_PREFILTER_MAP, m_SkyCapture.Prefiltered);

	// Send Shadow Maps
	// -----------------------------------------------------------------
	if (m_DirectionalLight != nullptr && m_DirectionalLight->Active && m_DirectionalLight->CastShadows)
	{
		for (size_t i = 0; i < m_ShadowMapper.cascades.size(); i++)
		{
			BindTexture(TEX_SHADOWMAP_1 + i, m_ShadowMapper.cascades[i].depthMap);
		}
	}
	/*if (!m_PointLights.empty())
	{
		for (size_t i = 0; i < m_PointShadowMapper.shadowMaps.size(); i++)
		{
			BindTexture(TEX_CUBE_SHADOWMAP + i, m_PointShadowMapper.GetShadowMap(i), true);
		}
	}
	if (!m_SpotLights.empty() && !m_SpotShadowMapper.shadowMaps.empty())
	{
		for (size_t i = 0; i < m_SpotShadowMapper.shadowMaps.size(); i++)
		{
			pbrShader->SetMat4(("spot_MVP[" + std::to_string(i) + "]").c_str(), m_SpotShadowMapper.shadowMaps[i]->MVP);
			BindTexture(TEX_CUBE_SHADOWMAP + 8 + i, m_SpotShadowMapper.GetShadowMap(i));
		}
	}
	*/
	// Draw
	// -------------------------------------------------------
	std::vector<Displacement*> Disps = mDisplacements.GetComponents();
	for (size_t i = 0; i < Disps.size(); i++)
	{
		BindMaterial(pbrShader, Disps[i]->mat0, true);
		//if(Disps[i]->mat1->tex_albedo != nullptr)
		//	m_cache.BindTexEmmi(Disps[i]->mat1->tex_albedo->getTexID());
		pbrShader->SetMat4("model", Disps[i]->model);
		Disps[i]->Render();
	}
}
// ------------------------------------------------------------------------
void Renderer::RenderScene(Camera& cam, bool static_only, GLuint target_frambuffer, int resolu)
{
	glm::mat4 proj = cam.GetProjectionMatrix();
	glm::mat4 view = cam.GetViewMatrix();
	glm::vec3 viewPos = cam.transform.Position;
	
	UpdateSkeletons(delTime);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glViewport(0, 0, SCR_weight, SCR_height);

	/*fplus_renderer.UpdateLights(m_PointLights);
	fplus_renderer.DepthPass(cam);
	//fplus_renderer.depthdebugShader.SetFloat("far", MainCam.FarView);
	//fplus_renderer.depthdebugShader.SetFloat("near", MainCam.NearView);
	for (size_t i = 0; i < m_RenderBuffer.OpaqueRenderCommands.size(); i++)
	{
		glUniformMatrix4fv(glGetUniformLocation(fplus_renderer.depthShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(m_RenderBuffer.OpaqueRenderCommands[i].Transform));
		m_RenderBuffer.OpaqueRenderCommands[i].Mesh->Draw();
	}
	fplus_renderer.LightCullingPass(cam);

	if (!cam.temp_cam) postProc.Bind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	*/
	/*glViewport(0, 0, SCR_weight, SCR_height);
	fplus_renderer.UpdateLights(m_PointLights);
	fplus_renderer.DepthPass(cam);
	for (size_t i = 0; i < m_RenderBuffer.OpaqueRenderCommands.size(); i++)
	{
		if (m_RenderBuffer.OpaqueRenderCommands[i].Material == nullptr)  continue;
		fplus_renderer.SetModel(m_RenderBuffer.OpaqueRenderCommands[i].Transform);
		m_RenderBuffer.OpaqueRenderCommands[i].Mesh->Draw();
	}
	fplus_renderer.LightCullingPass(cam);
	glViewport(left_scr_pos, 0, SCR_weight, SCR_height);
	fplus_renderer.debugShader.use();
	fplus_renderer.debugShader.setInt("numberOfTilesX", fplus_renderer.workGroupsX);
	fplus_renderer.debugShader.setInt("totalLightCount", fplus_renderer.NUM_LIGHTS);
	glUniformMatrix4fv(glGetUniformLocation(fplus_renderer.debugShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(cam.GetProjectionMatrix()));
	glUniformMatrix4fv(glGetUniformLocation(fplus_renderer.debugShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(cam.GetViewMatrix()));
	glUniform3fv(glGetUniformLocation(fplus_renderer.debugShader.Program, "viewPosition"), 1, &cam.transform.Position[0]);
	//std::cout << cam.transform.Position.x << " " << cam.transform.Position.y << " " << cam.transform.Position.z << "\n";
	for (size_t i = 0; i < m_RenderBuffer.OpaqueRenderCommands.size(); i++)
	{
		glUniformMatrix4fv(glGetUniformLocation(fplus_renderer.debugShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(m_RenderBuffer.OpaqueRenderCommands[i].Transform));
		m_RenderBuffer.OpaqueRenderCommands[i].Mesh->Draw();
	}

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);

	m_RenderBuffer.Clear();

	MainCam.FarView = 50;
	postProc.mm = fplus_renderer.depthMap;
	return;
	*/

	/*glViewport(0, 0, SCR_weight, SCR_height);
	fplus_renderer.UpdateLights(m_PointLights);
	glBindFramebuffer(GL_FRAMEBUFFER, fplus_renderer.depthMapFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	materials.DepthShader.use();
	materials.DepthShader.SetMat4("lightSpaceMatrix", cam.GetProjectionMatrix() *cam.GetViewMatrix());

	for (size_t j = 0; j < m_RenderBuffer.OpaqueRenderCommands.size(); j++)
	{
		materials.DepthShader.SetMat4("model", m_RenderBuffer.OpaqueRenderCommands[j].Transform);
		m_RenderBuffer.OpaqueRenderCommands[j].Mesh->Draw();
	}*/

	// SSAO Post Process
	// -----------------------------------------------------------------
	if (!cam.temp_cam && postProc.Initialized && postProc.Use && postProc.use_ssao)
	{
		// geometry pass: render scene's geometry/color data into gbuffer
		// -----------------------------------------------------------------
		postProc.gBuffer.Bind();
		materials.ShaderGeometryPass.use();
		materials.ShaderGeometryPass.SetMat4("projection", proj);
		materials.ShaderGeometryPass.SetMat4("view", view);		

		for (size_t i = 0; i < m_RenderBuffer.OpaqueRenderCommands.size(); i++)
		{
			if (m_RenderBuffer.OpaqueRenderCommands[i].Material == nullptr)  continue;
			materials.ShaderGeometryPass.SetMat4("model", m_RenderBuffer.OpaqueRenderCommands[i].Transform);
			m_RenderBuffer.OpaqueRenderCommands[i].Mesh->Draw();
		}
		//postProc.gBuffer.UnBind();

		// generate SSAO texture
		// ------------------------
		postProc.ssaoEffect.Bind(proj);
		/*postProc.ssaoEffect.shaderSSAO->setInt("gDepthMap", 4);
		postProc.ssaoEffect.shaderSSAO->SetMat4("invProj", glm::inverse(cam.GetProjectionMatrix()));
		BindTexture(4, fplus_renderer.depthMap);*/
		BindTexture(5, postProc.gBuffer.gPosition);
		BindTexture(6, postProc.gBuffer.gNormal);
		BindTexture(7, postProc.ssaoEffect.noiseTexture);
		renderQuad();
		//postProc.ssaoEffect.UnBind();

		// blur SSAO texture to remove noise
		// ------------------------------------
		glViewport(0, 0, (int)postProc.ssaoEffect.vBlur.scr_w, (int)postProc.ssaoEffect.vBlur.scr_h);

		postProc.ssaoEffect.vBlur.Bind();
		glClear(GL_COLOR_BUFFER_BIT);
		postProc.blurShader.use();
		postProc.blurShader.setBool("horizontal", false);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, postProc.ssaoEffect.ssaoColorBuffer);
		renderQuad();

		postProc.ssaoEffect.hBlur.Bind();
		glClear(GL_COLOR_BUFFER_BIT);
		postProc.blurShader.setBool("horizontal", true);
		postProc.ssaoEffect.vBlur.UseTexture();
		renderQuad();

		//postProc.ssaoEffect.Blur();
		//renderQuad();
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	// Check if We have enough render commands to draw
	// -----------------------------------------------------------------
	//if (m_RenderBuffer.OpaqueRenderCommands.size() > 0)
	//{
	glActiveTexture(GL_TEXTURE0);
	// Directional Light Shadows Mapping (Cascaded Shadow Mapping)
	// -----------------------------------------------------------------
	if (m_DirectionalLight != nullptr && m_DirectionalLight->Active && m_DirectionalLight->CastShadows)
	{
		m_ShadowMapper.CalcOrthoProjs(&cam, m_DirectionalLight);
		
		for (size_t i = 0; i < m_ShadowMapper.cascades.size(); i++)
		{
			frustum.Update(m_ShadowMapper.cascades[i].LightViewProjection);
			m_ShadowMapper.Bind(i);
			materials.DepthShader.use();
			materials.DepthShader.SetMat4("lightSpaceMatrix", m_ShadowMapper.cascades[i].LightViewProjection);
			for (size_t i = 0; i < m_RenderBuffer.OpaqueRenderCommands.size(); i++)
			{
				if (static_only && !m_RenderBuffer.OpaqueRenderCommands[i].is_static) continue;
				if (!m_RenderBuffer.OpaqueRenderCommands[i].cast_shadows) continue;
				if (!frustum.IsBoxVisible(m_RenderBuffer.OpaqueRenderCommands[i].bbox.BoxMin, m_RenderBuffer.OpaqueRenderCommands[i].bbox.BoxMax)) continue;

				materials.DepthShader.SetMat4("model", m_RenderBuffer.OpaqueRenderCommands[i].Transform);
				m_RenderBuffer.OpaqueRenderCommands[i].Mesh->Draw();
			}
			materials.DepthShader.setBool("use_alpha", true);
			for (size_t i = 0; i < m_RenderBuffer.CutoutRenderCommands.size(); i++)
			{
				if (static_only && !m_RenderBuffer.CutoutRenderCommands[i].is_static) continue;
				if (!m_RenderBuffer.CutoutRenderCommands[i].cast_shadows) continue;
				if (!frustum.IsBoxVisible(m_RenderBuffer.CutoutRenderCommands[i].bbox.BoxMin, m_RenderBuffer.OpaqueRenderCommands[i].bbox.BoxMax)) continue;

				if(m_RenderBuffer.CutoutRenderCommands[i].Material->tex_emission != nullptr)
					m_RenderBuffer.CutoutRenderCommands[i].Material->tex_emission->useTexture();

				materials.DepthShader.SetMat4("model", m_RenderBuffer.CutoutRenderCommands[i].Transform);
				m_RenderBuffer.CutoutRenderCommands[i].Mesh->Draw();
			}
			materials.DepthShader.setBool("use_alpha", false);

			materials.DepthShader_sk.use();
			materials.DepthShader_sk.SetMat4("lightSpaceMatrix", m_ShadowMapper.cascades[i].LightViewProjection);
			auto anims = m_skMeshs.GetComponents();
			for (size_t i = 0; i < anims.size(); i++)
			{
				if (!anims[i]->enabled || anims[i]->mesh == nullptr) continue;

				auto transforms = anims[i]->animator.GetFinalBoneMatrices();
				for (size_t i = 0; i < transforms.size(); ++i)
					materials.DepthShader_sk.SetMat4(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), transforms[i]);

				materials.DepthShader_sk.SetMat4("model", glm::scale(anims[i]->transform, glm::vec3(anims[i]->scale)));
				anims[i]->Draw();
			}
			m_ShadowMapper.Unbind();
			if (cam.temp_cam)
			{
				glViewport(0, 0, (int)resolu, (int)resolu);
				glBindFramebuffer(GL_FRAMEBUFFER, target_frambuffer);
			}

		}
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// Point Lights Shadows Mapping.
	// -----------------------------------------------------------------
	frustum.Update(MainCam.GetProjectionMatrix() * MainCam.GetViewMatrix());
	if (!m_PointLights.empty())
	{
		for (size_t i = 0; i < m_PointLights.size(); i++)
		{
			if (m_PointLights[i]->CastShadows == true && m_PointLights[i]->shadow_index != -1)
			{
				m_PointLights[i]->visible = m_PointLights[i]->inFrustum;//frustum.sphereInFrustum(m_PointLights[i]->Position, m_PointLights[i]->Raduis);
				if (!m_PointLights[i]->visible || !m_PointLights[i]->isActive())
				{
					not_visible.push_back(i);
					continue;
				}
				if (m_PointLights[i]->Static && m_PointLights[i]->baked)
					continue;
				else if (m_PointLights[i]->Static && !m_PointLights[i]->baked)
					m_PointLights[i]->baked = true;

				if (glm::distance(m_PointLights[i]->Position, MainCam.transform.Position) > m_ShadowMapper.GetShadowDistance())
					continue;
				// render scene to depth cubemap
				// --------------------------------
				m_PointShadowMapper.Bind(materials.PointDepthShader, m_PointLights[i]->shadow_index, m_PointLights[i]->Position, m_PointLights[i]->Raduis);

				for (size_t j = 0; j < m_RenderBuffer.OpaqueRenderCommands.size(); j++)
				{
					if (!m_RenderBuffer.OpaqueRenderCommands[j].cast_shadows) continue;
					if (static_only && !m_RenderBuffer.OpaqueRenderCommands[j].is_static) continue;
					//if (Check2BallsIntersect(m_PointLights[i]->Position, m_PointLights[i]->Raduis, m_RenderBuffer.OpaqueRenderCommands[j].position, m_RenderBuffer.OpaqueRenderCommands[j].bbox.radius))
					//	continue;

					materials.PointDepthShader.SetMat4("model", m_RenderBuffer.OpaqueRenderCommands[j].Transform);
					m_RenderBuffer.OpaqueRenderCommands[j].Mesh->Draw();
				}
				m_PointShadowMapper.Unbind();
				if (cam.temp_cam)
				{
					glViewport(0, 0, (int)resolu, (int)resolu);
					glBindFramebuffer(GL_FRAMEBUFFER, target_frambuffer);
				}
			}

			if (m_PointLights[i]->CastShadows == false && m_PointLights[i]->shadow_index != -1)
			{
				m_PointShadowMapper.RemoveShadowMap(m_PointLights[i]->shadow_index);
				m_PointLights[i]->shadow_index = -1;
				ReIndexPointLightsShadowMaps();
			}
			// if a light cast shadow then create a shadow map for it.
			else if (m_PointShadowMapper.shadowMaps.size() < 8 && m_PointLights[i]->CastShadows == true && m_PointLights[i]->shadow_index == -1)
			{
				m_PointLights[i]->shadow_index = m_PointShadowMapper.CreateShadowMap()->index;
			}
		}
		// Switch shadowmaps at the time the light is not visible to the camera to another visible light.
		if (!not_visible.empty())
		{
			int last_not_used = 0;
			for (size_t i = 0; i < m_PointLights.size(); i++)
			{
				if (m_PointLights[i]->CastShadows == true && m_PointLights[i]->shadow_index == -1)
				{
					m_PointLights[i]->shadow_index = m_PointLights[not_visible[last_not_used]]->shadow_index;
					m_PointLights[not_visible[last_not_used]]->shadow_index = -1;
					m_PointLights[not_visible[last_not_used]]->baked = false;
					last_not_used++;
					if (last_not_used == not_visible.size())
						break;
				}
			}
			not_visible.clear();
		}
	}
	// Spot Lights Shadows Mapping.
	// -----------------------------------------------------------------
	if (!m_SpotLights.empty())
	{
		for (size_t i = 0; i < m_SpotLights.size(); i++)
		{
			if (!m_SpotLights[i]->removed && m_SpotLights[i]->CastShadows == true && m_SpotLights[i]->shadow_index != -1)
			{
				m_SpotLights[i]->visible = m_SpotLights[i]->inFrustum;//frustum.sphereInFrustum(m_SpotLights[i]->Position, m_SpotLights[i]->Raduis);
				if (!m_SpotLights[i]->visible || !m_SpotLights[i]->isActive())
				{
					not_visible.push_back(i);
					continue;
				}
				if (m_SpotLights[i]->Static && m_SpotLights[i]->baked)
					continue;
				else if (m_SpotLights[i]->Static && !m_SpotLights[i]->baked)
					m_SpotLights[i]->baked = true;

				if (glm::distance(m_SpotLights[i]->Position, MainCam.transform.Position) > m_ShadowMapper.GetShadowDistance())
					continue;
				// render scene to depth texture
				// --------------------------------
				glm::mat4 mats = m_SpotShadowMapper.Bind(m_SpotLights[i]->shadow_index, m_SpotLights[i]->Position, m_SpotLights[i]->Direction, m_SpotLights[i]->Raduis, m_SpotLights[i]->CutOff + m_SpotLights[i]->OuterCutOff);
				Frustum fr; fr.Update(mats);

				materials.DepthShader.use();
				materials.DepthShader.SetMat4("lightSpaceMatrix", mats);

				for (size_t j = 0; j < m_RenderBuffer.OpaqueRenderCommands.size(); j++)
				{
					if (!m_RenderBuffer.OpaqueRenderCommands[j].cast_shadows) continue;
					if (!fr.IsBoxVisible(m_RenderBuffer.OpaqueRenderCommands[j].bbox.BoxMin, m_RenderBuffer.OpaqueRenderCommands[j].bbox.BoxMax)) continue;

					materials.DepthShader.SetMat4("model", m_RenderBuffer.OpaqueRenderCommands[j].Transform);
					m_RenderBuffer.OpaqueRenderCommands[j].Mesh->Draw();
				}

				materials.DepthShader_sk.use();
				materials.DepthShader_sk.SetMat4("lightSpaceMatrix", mats);
				auto anims = m_skMeshs.GetComponents();
				for (size_t i = 0; i < anims.size(); i++)
				{
					if (!anims[i]->enabled || anims[i]->mesh == nullptr) continue;

					auto transforms = anims[i]->animator.GetFinalBoneMatrices();
					for (size_t i = 0; i < transforms.size(); ++i)
						materials.DepthShader_sk.SetMat4(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), transforms[i]);

					materials.DepthShader_sk.SetMat4("model", glm::scale(anims[i]->transform, glm::vec3(anims[i]->scale)));
					anims[i]->Draw();
				}

				m_SpotShadowMapper.Unbind();

				if (cam.temp_cam)
				{
					glViewport(0, 0, (int)resolu, (int)resolu);
					glBindFramebuffer(GL_FRAMEBUFFER, target_frambuffer);
				}
			}

			if (m_SpotLights[i]->removed || m_SpotLights[i]->CastShadows == false && m_SpotLights[i]->shadow_index != -1)
			{
				m_SpotShadowMapper.RemoveShadowMap(m_SpotLights[i]->shadow_index);
				m_SpotLights[i]->shadow_index = -1;
				ReIndexSpotLightsShadowMaps();
			}

			// if a light cast shadow then create a shadow map for it.
			else if (m_SpotShadowMapper.shadowMaps.size() < 8 && m_SpotLights[i]->CastShadows == true && m_SpotLights[i]->shadow_index == -1)
			{
				std::cout << "Creating shadow map for spot light by request \n";
				m_SpotLights[i]->shadow_index = m_SpotShadowMapper.CreateShadowMap()->index;
			}
		}

		// Switch shadowmaps at the time the light is not visible to the camera to another visible light.
		if (!not_visible.empty())
		{
			int last_not_used = 0;
			for (size_t i = 0; i < m_SpotLights.size(); i++)
			{
				if (m_SpotLights[i]->CastShadows == true && m_SpotLights[i]->shadow_index == -1)
				{
					m_SpotLights[i]->shadow_index = m_SpotLights[not_visible[last_not_used]]->shadow_index;
					m_SpotLights[not_visible[last_not_used]]->shadow_index = -1;
					m_SpotLights[not_visible[last_not_used]]->baked = false;
					last_not_used++;
					if (last_not_used == not_visible.size())
						break;
				}
			}
			not_visible.clear();
		}
	}

	if (!cam.temp_cam)
	{
		if (postProc.Use)
			glViewport(0, 0, SCR_weight, SCR_height);
		else
			glViewport(left_scr_pos, 0, SCR_weight, SCR_height);
	}

	// Final Render Pass
	// -----------------------------------------------------------------
	
	//fplus_renderer.LightCullingPass(cam);

	// Bind Post Proccessing if used
	if (!cam.temp_cam) postProc.Bind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Shader* pbrShader = &materials.PbrShader;

	m_cache.SetCamera(cam);

	// Send Directional Light informations to shader
	// -----------------------------------------------------------------
	if (m_DirectionalLight != nullptr)
	{
		if (m_DirectionalLight->removed)
		{
			RemoveDirectionalLight();
		}
		else if (m_DirectionalLight->Active)
		{
			pbrShader->setBool("dirLight.cast_shadows", m_DirectionalLight->CastShadows);
			if (m_DirectionalLight->CastShadows)
			{
				for (size_t i = 0; i < m_ShadowMapper.cascades.size(); i++)
				{
					pbrShader->SetMat4(("gLightWVP[" + std::to_string(i) + "]").c_str(), m_ShadowMapper.cascades[i].LightViewProjection);
					pbrShader->SetFloat(("CascadeEndClipSpace[" + std::to_string(i) + "]").c_str(), m_ShadowMapper.cascades[i].cascadeSplitFar);
				}
			}
			pbrShader->SetVec3("dirLight.direction", m_DirectionalLight->Direction);
			pbrShader->SetFloat("dirLight.intensity", m_DirectionalLight->Intensity);
			pbrShader->SetVec3("dirLight.color", m_DirectionalLight->Color);
			pbrShader->setBool("dirLight.soft_shadows", m_DirectionalLight->Soft);
			pbrShader->SetFloat("dirLight.Bias", m_DirectionalLight->Bias);
		}
	}
	int visibe_lights_index = 0;

	// Send Point Lights informations to shader
	// -----------------------------------------------------------------
	for (unsigned int i = 0; i < MAX_LIGHT_COUNT; i++)
	{
		if (m_PointLights.empty())
			break;
		if ((m_PointLights.size() - 1) >= i)
		{
			if (m_PointLights[i]->removed)
			{
				RemovePointLight(m_PointLights[i]->light_id);
				continue;
			}
			if (m_PointLights[i]->isActive())
			{
				m_PointLights[i]->inFrustum = frustum.sphereInFrustum(m_PointLights[i]->Position, m_PointLights[i]->Raduis);
				
				if (m_PointLights[i]->inFrustum) {

					pbrShader->setInt(("visible_pLights[" + std::to_string(visibe_lights_index) + "]").c_str(), i);
					visibe_lights_index++;

					pbrShader->SetVec4(("p_lights[" + std::to_string(i) + "].position").c_str(), glm::vec4(m_PointLights[i]->Position, m_PointLights[i]->Raduis));
					pbrShader->SetVec4(("p_lights[" + std::to_string(i) + "].color").c_str(), glm::vec4(m_PointLights[i]->Color, m_PointLights[i]->Intensity));
					pbrShader->SetFloat(("p_lights[" + std::to_string(i) + "].cast_shadows").c_str(), m_PointLights[i]->CastShadows);

					if (m_PointLights[i]->CastShadows) {
						pbrShader->SetFloat(("p_lights[" + std::to_string(i) + "].Bias").c_str(), m_PointLights[i]->Bias);
						pbrShader->setInt(("p_lights[" + std::to_string(i) + "].shadow_index").c_str(), m_PointLights[i]->shadow_index);
					}
				}
			}
		}
		else
			break;
	}

	if(visibe_lights_index < MAX_LIGHT_COUNT)
		pbrShader->setInt(("visible_pLights[" + std::to_string(visibe_lights_index) + "]").c_str(), -1);

	visibe_lights_index = 0;

	// Send Spot Lights informations to shader
	// -----------------------------------------------------------------
	for (unsigned int i = 0; i < MAX_LIGHT_COUNT; i++)
	{
		if (m_SpotLights.empty())
			break;
		if ((m_SpotLights.size() - 1) >= i)
		{
			if (m_SpotLights[i]->removed)
			{
				RemoveSpotLight(m_SpotLights[i]->light_id);
				continue;
			}
			if (m_SpotLights[i]->isActive())
			{
				m_SpotLights[i]->inFrustum = frustum.sphereInFrustum(m_SpotLights[i]->Position, m_SpotLights[i]->Raduis);
				if (m_SpotLights[i]->inFrustum) {

					pbrShader->setInt(("visible_sLights[" + std::to_string(visibe_lights_index) + "]").c_str(), i);
					visibe_lights_index++;

					pbrShader->SetVec4(("sp_lights[" + std::to_string(i) + "].direction").c_str(), glm::vec4(m_SpotLights[i]->Direction, m_SpotLights[i]->CutOff));
					pbrShader->SetVec4(("sp_lights[" + std::to_string(i) + "].position").c_str(), glm::vec4(m_SpotLights[i]->Position, m_SpotLights[i]->Raduis));
					pbrShader->SetVec4(("sp_lights[" + std::to_string(i) + "].color").c_str(), glm::vec4(m_SpotLights[i]->Color, m_SpotLights[i]->Intensity));
					pbrShader->SetFloat(("sp_lights[" + std::to_string(i) + "].outerCutOff").c_str(), glm::cos(glm::radians(m_SpotLights[i]->CutOff + m_SpotLights[i]->OuterCutOff)));

					pbrShader->SetFloat(("sp_lights[" + std::to_string(i) + "].cast_shadows").c_str(), m_SpotLights[i]->CastShadows);
					pbrShader->SetFloat(("sp_lights[" + std::to_string(i) + "].Bias").c_str(), m_SpotLights[i]->Bias);
					pbrShader->setInt(("sp_lights[" + std::to_string(i) + "].shadow_index").c_str(), m_SpotLights[i]->shadow_index);
				}
			}
		}
		else
			break;
	}
	if (visibe_lights_index < MAX_LIGHT_COUNT)
		pbrShader->setInt(("visible_sLights[" + std::to_string(visibe_lights_index) + "]").c_str(), -1);

	for (size_t i = 0; i < m_ReflectionProbes.size(); i++)
	{
		if (m_ReflectionProbes[i]->removed)
			RemoveReflectionProbe(m_ReflectionProbes[i]->probe_id);
	}
	// Set Fog config
	// -----------------------------------------------------------------
	pbrShader->setBool("Fog.use", usefog);
	if (usefog)
	{
		pbrShader->SetVec3("Fog.color", fogColor);
		pbrShader->SetFloat("Fog.far", fogFar);
		pbrShader->SetFloat("Fog.near", fogNear);
	}
	// Send PBR environments maps
	// -----------------------------------------------------------------
	m_cache.BindTexture(TEX_IRRADIANCE_MAP, m_SkyCapture.Irradiance);
	m_cache.BindTexture(TEX_PREFILTER_MAP, m_SkyCapture.Prefiltered);

	// Send Shadow Maps
	// -----------------------------------------------------------------
	if (m_DirectionalLight != nullptr && m_DirectionalLight->Active && m_DirectionalLight->CastShadows)
	{
		for (size_t i = 0; i < m_ShadowMapper.cascades.size(); i++)
		{
			BindTexture(TEX_SHADOWMAP_1 + i, m_ShadowMapper.cascades[i].depthMap);
		}
	}
	if (!m_PointLights.empty())
	{
		for (size_t i = 0; i < m_PointShadowMapper.shadowMaps.size(); i++)
		{
			BindTexture(TEX_CUBE_SHADOWMAP + i, m_PointShadowMapper.GetShadowMap(i), true);
		}
	}
	if (!m_SpotLights.empty() && !m_SpotShadowMapper.shadowMaps.empty())
	{
		for (size_t i = 0; i < m_SpotShadowMapper.shadowMaps.size(); i++)
		{
			pbrShader->SetMat4(("spot_MVP[" + std::to_string(i) + "]").c_str(), m_SpotShadowMapper.shadowMaps[i]->MVP);
			BindTexture(TEX_CUBE_SHADOWMAP + 8 + i, m_SpotShadowMapper.GetShadowMap(i));
		}
	}

	std::string lastMaterial = "";

	// Render all pushed commands
	// ----------------------------------------------------
	for (size_t i = 0; i < m_RenderBuffer.OpaqueRenderCommands.size(); i++)
	{
		if (static_only && !m_RenderBuffer.OpaqueRenderCommands[i].is_static) continue;
		if (m_RenderBuffer.OpaqueRenderCommands[i].Material == nullptr) continue;

		// Frustum culling
		// -------------------------------------------------------
		if (!frustum.IsBoxVisible(m_RenderBuffer.OpaqueRenderCommands[i].bbox.BoxMin, m_RenderBuffer.OpaqueRenderCommands[i].bbox.BoxMax))
			continue;

		// Bind Material
		// -------------------------------------------------------
		if (m_RenderBuffer.OpaqueRenderCommands[i].Material->isDefault)
			lastMaterial = "default-mat";

		if (lastMaterial != m_RenderBuffer.OpaqueRenderCommands[i].Material->name)
		{
			BindMaterial(pbrShader, m_RenderBuffer.OpaqueRenderCommands[i].Material);
			lastMaterial = m_RenderBuffer.OpaqueRenderCommands[i].Material->name;
		}

		// Check ReflectionProbe intersection
		// -------------------------------------------------------
		bool intersectionFound = false;
		if (!cam.temp_cam)
		{
			for (size_t j = 0; j < m_ReflectionProbes.size(); j++)
			{
				if (m_ReflectionProbes[j]->GetBBox().Intersect(m_RenderBuffer.OpaqueRenderCommands[i].bbox))
				{
					intersectionFound = true;
					pbrShader->setBool("env_probe.use_parallax_correction", m_ReflectionProbes[j]->BoxProjection);
					pbrShader->SetVec3("env_probe.mRefPos", m_ReflectionProbes[j]->Position);
					pbrShader->SetVec3("env_probe.mBoxMin", m_ReflectionProbes[j]->GetBBox().BoxMin);
					pbrShader->SetVec3("env_probe.mBoxMax", m_ReflectionProbes[j]->GetBBox().BoxMax);
					m_cache.BindTexture(TEX_IRRADIANCE_MAP, m_ReflectionProbes[j]->capture.Irradiance);
					m_cache.BindTexture(TEX_PREFILTER_MAP, m_ReflectionProbes[j]->capture.Prefiltered);
					break;
				}
			}
			if (!intersectionFound)
			{
				pbrShader->setBool("env_probe.use_parallax_correction", false);
				m_cache.BindTexture(TEX_IRRADIANCE_MAP, m_SkyCapture.Irradiance);
				m_cache.BindTexture(TEX_PREFILTER_MAP, m_SkyCapture.Prefiltered);
			}
		}
		else pbrShader->setBool("env_probe.use_parallax_correction", false);

		// Bind Lightmap if availiable 
		// -------------------------------------------------------
		if (m_RenderBuffer.OpaqueRenderCommands[i].is_static && m_RenderBuffer.OpaqueRenderCommands[i].lightmapPath != "")
		{
			pbrShader->setBool("use_lightmap", true);
			glActiveTexture(GL_TEXTURE6);
			Get_Lightmap(m_RenderBuffer.OpaqueRenderCommands[i].lightmapPath)->useTexture();
		}
		else pbrShader->setBool("use_lightmap", false);

		// Draw
		// -------------------------------------------------------
		pbrShader->SetMat4("model", m_RenderBuffer.OpaqueRenderCommands[i].Transform);
		m_RenderBuffer.OpaqueRenderCommands[i].Mesh->Draw();
	}

	if (!m_RenderBuffer.CutoutRenderCommands.empty())
	{
		pbrShader->setBool("use_alpha", true);
		glDisable(GL_CULL_FACE);
	}
	for (size_t i = 0; i < m_RenderBuffer.CutoutRenderCommands.size(); i++)
	{
		if (static_only && !m_RenderBuffer.CutoutRenderCommands[i].is_static) continue;
		if (m_RenderBuffer.CutoutRenderCommands[i].Material == nullptr) continue;

		// Frustum culling
		// -------------------------------------------------------
		if (!frustum.IsBoxVisible(m_RenderBuffer.CutoutRenderCommands[i].bbox.BoxMin, m_RenderBuffer.CutoutRenderCommands[i].bbox.BoxMax))
			continue;

		// Bind Material
		// -------------------------------------------------------
		if (m_RenderBuffer.CutoutRenderCommands[i].Material->isDefault)
			lastMaterial = "default-mat";

		if (lastMaterial != m_RenderBuffer.CutoutRenderCommands[i].Material->name)
		{
			BindMaterial(pbrShader, m_RenderBuffer.CutoutRenderCommands[i].Material);
			lastMaterial = m_RenderBuffer.CutoutRenderCommands[i].Material->name;
		}

		// Check ReflectionProbe intersection
		// -------------------------------------------------------
		bool intersectionFound = false;
		if (!cam.temp_cam)
		{
			for (size_t j = 0; j < m_ReflectionProbes.size(); j++)
			{
				if (m_ReflectionProbes[j]->GetBBox().Intersect(m_RenderBuffer.CutoutRenderCommands[i].bbox))
				{
					intersectionFound = true;
					pbrShader->setBool("env_probe.use_parallax_correction", m_ReflectionProbes[j]->BoxProjection);
					pbrShader->SetVec3("env_probe.mRefPos", m_ReflectionProbes[j]->Position);
					pbrShader->SetVec3("env_probe.mBoxMin", m_ReflectionProbes[j]->GetBBox().BoxMin);
					pbrShader->SetVec3("env_probe.mBoxMax", m_ReflectionProbes[j]->GetBBox().BoxMax);
					m_cache.BindTexture(TEX_IRRADIANCE_MAP, m_ReflectionProbes[j]->capture.Irradiance);
					m_cache.BindTexture(TEX_PREFILTER_MAP, m_ReflectionProbes[j]->capture.Prefiltered);
					break;
				}
			}
			if (!intersectionFound)
			{
				pbrShader->setBool("env_probe.use_parallax_correction", false);
				m_cache.BindTexture(TEX_IRRADIANCE_MAP, m_SkyCapture.Irradiance);
				m_cache.BindTexture(TEX_PREFILTER_MAP, m_SkyCapture.Prefiltered);
			}
		}
		else pbrShader->setBool("env_probe.use_parallax_correction", false);

		// Bind Lightmap if availiable 
		// -------------------------------------------------------
		if (m_RenderBuffer.CutoutRenderCommands[i].is_static && m_RenderBuffer.CutoutRenderCommands[i].lightmapPath != "")
		{
			pbrShader->setBool("use_lightmap", true);
			glActiveTexture(GL_TEXTURE6);
			Get_Lightmap(m_RenderBuffer.CutoutRenderCommands[i].lightmapPath)->useTexture();
		}
		else pbrShader->setBool("use_lightmap", false);

		// Draw
		// -------------------------------------------------------
		pbrShader->SetMat4("model", m_RenderBuffer.CutoutRenderCommands[i].Transform);
		m_RenderBuffer.CutoutRenderCommands[i].Mesh->Draw();
	}

	if (!m_RenderBuffer.CutoutRenderCommands.empty())
	{
		pbrShader->setBool("use_alpha", false);
		glEnable(GL_CULL_FACE);
	}

	if (!cam.temp_cam)
		m_RenderBuffer.Clear();

	RenderDisplacements();

	//}
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
	// Skeletal meshes
	//-----------------------------------------------------
	RenderSkeletons(delTime);

	// render skybox (render as last to prevent overdraw)
	// ----------------------------------------------------
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	Shader* bg = &materials.background;
	bg->use();
	bg->SetMat4("view", view);
	bg->SetMat4("projection", proj);
	if (m_DirectionalLight != nullptr)
	{
		bg->SetVec3("sun_direction", m_DirectionalLight->Direction);
	}
	m_cache.BindTexture(TEX_IRRADIANCE_MAP, envCubemap);

	bg->SetFloat("UseClouds", UseClouds);
	if(UseClouds)
		m_cache.BindTexture(TEX_PREFILTER_MAP, skyClouds);

	glDisable(GL_CULL_FACE);
	renderCube();
	glEnable(GL_CULL_FACE);
	//renderSphere();
	glDepthFunc(GL_LESS); // set depth function back to default
}
// ------------------------------------------------------------------------
void Renderer::BindMaterial(Shader* pbrShader, Material* mat, bool isDisp)
{
	pbrShader->SetVec2("tex_uv", mat->uv);
	// albedo map 
	// -------------------------------------------------------
	if (mat->tex_albedo != nullptr)
	{
		pbrShader->setBool("material.use_tex_albedo", true);
		m_cache.BindTexAlbedo(mat->tex_albedo->getTexID());
	}
	else pbrShader->setBool("material.use_tex_albedo", false);

	// metalic map 
	// -------------------------------------------------------
	if (mat->tex_metal != nullptr)
	{
		pbrShader->setBool("material.use_tex_metal", true);
		m_cache.BindTexMetal(mat->tex_metal->getTexID());
	}
	else pbrShader->setBool("material.use_tex_metal", false);
	// roughness map 
	// -------------------------------------------------------
	if (mat->tex_rough != nullptr)
	{
		pbrShader->setBool("material.use_tex_rough", true);
		m_cache.BindTexRough(mat->tex_rough->getTexID());
	}
	else pbrShader->setBool("material.use_tex_rough", false);

	// normal map 
	// -------------------------------------------------------
	if (mat->tex_normal != nullptr)
	{
		pbrShader->setBool("material.use_tex_normal", true);
		m_cache.BindTexNormal(mat->tex_normal->getTexID());
	}
	else pbrShader->setBool("material.use_tex_normal", false);

	// Emission 
	// -------------------------------------------------------
	if (mat->cutout || isDisp)
	{
		bool ht = (mat->tex_emission != nullptr);
		pbrShader->setBool("material.use_emission", ht);
		if (ht)
			m_cache.BindTexEmmi(mat->tex_emission->getTexID());
	}

	if (!isDisp && mat->use_emission)
	{
		pbrShader->setBool("material.use_emission", true);
		pbrShader->SetVec3("material.emission", mat->emission);
		pbrShader->SetFloat("material.emission_power", mat->emission_power);

		if (mat->tex_emission != nullptr)
		{
			pbrShader->setBool("material.use_tex_emission", true);
			m_cache.BindTexEmmi(mat->tex_emission->getTexID());
		}
		else pbrShader->setBool("material.use_tex_emission", false);
	}
	else pbrShader->setBool("material.use_emission", false);

	mat->setShader(pbrShader);
	pbrShader->SetFloat("material.ao", mat->ao * AmbientLevel);
}
// ------------------------------------------------------------------------
void Renderer::RenderSkeletons(float dt)
{
	/*static bool init = false;

	static SK_Model ourModel;
	static Animation RunAnimation;
	static Animator animator;
	static Texture tex;
	if (!init)
	{
		init = true;
		ourModel.loadModel(std::string("res\\models\\Charactar\\Idle.fbx"));
		RunAnimation.LoadAnimation(std::string("res\\models\\Charactar\\Running.fbx"), &ourModel);
		animator.LoadAnimation(&RunAnimation);
		tex.setTexture("res\\models\\Charactar\\textures\\diffuse.png", "res\\models\\Charactar\\textures\\diffuse.png", false);
	}

	animator.UpdateAnimation(delTime);*/

	Shader* pbrShader = &materials.SkelShader;
	pbrShader->use();
	pbrShader->SetMat4("view", MainCam.GetViewMatrix());
	pbrShader->SetMat4("projection", MainCam.GetProjectionMatrix());
	pbrShader->SetVec3("CamPos", MainCam.transform.Position);

	// Send Directional Light informations to shader
	// -----------------------------------------------------------------
	if (m_DirectionalLight != nullptr)
	{
		if (m_DirectionalLight->Active)
		{
			pbrShader->setBool("dirLight.cast_shadows", m_DirectionalLight->CastShadows);
			if (m_DirectionalLight->CastShadows)
			{
				for (size_t i = 0; i < m_ShadowMapper.cascades.size(); i++)
				{
					pbrShader->SetMat4(("gLightWVP[" + std::to_string(i) + "]").c_str(), m_ShadowMapper.cascades[i].LightViewProjection);
					pbrShader->SetFloat(("CascadeEndClipSpace[" + std::to_string(i) + "]").c_str(), m_ShadowMapper.cascades[i].cascadeSplitFar);
				}
			}
			pbrShader->SetVec3("dirLight.direction", m_DirectionalLight->Direction);
			pbrShader->SetFloat("dirLight.intensity", m_DirectionalLight->Intensity);
			pbrShader->SetVec3("dirLight.color", m_DirectionalLight->Color);
			pbrShader->setBool("dirLight.soft_shadows", m_DirectionalLight->Soft);
			pbrShader->SetFloat("dirLight.Bias", m_DirectionalLight->Bias);
		}
		pbrShader->SetFloat("dirLight.use", m_DirectionalLight->Active);
	}
	else pbrShader->SetFloat("dirLight.use", false);

	// Send Point Lights informations to shader
	// -----------------------------------------------------------------
	for (unsigned int i = 0; i < MAX_LIGHT_COUNT_SK; i++)
	{
		if (m_PointLights.empty())
			break;
		if ((m_PointLights.size() - 1) >= i)
		{
			pbrShader->SetVec3(("p_lights[" + std::to_string(i) + "].position").c_str(), m_PointLights[i]->Position);
			pbrShader->SetVec3(("p_lights[" + std::to_string(i) + "].color").c_str(), m_PointLights[i]->Color);
			pbrShader->SetFloat(("p_lights[" + std::to_string(i) + "].intensity").c_str(), m_PointLights[i]->Intensity);
			pbrShader->SetFloat(("p_lights[" + std::to_string(i) + "].raduis").c_str(), m_PointLights[i]->Raduis);
			pbrShader->SetFloat(("p_lights[" + std::to_string(i) + "].cast_shadows").c_str(), m_PointLights[i]->CastShadows);
			pbrShader->SetFloat(("p_lights[" + std::to_string(i) + "].Bias").c_str(), m_PointLights[i]->Bias);
			pbrShader->setInt(("p_lights[" + std::to_string(i) + "].shadow_index").c_str(), m_PointLights[i]->shadow_index);
			pbrShader->setInt(("p_lights[" + std::to_string(i) + "].use").c_str(), m_PointLights[i]->Active);
			pbrShader->setBool(("p_lights[" + std::to_string(i) + "].offscreen").c_str(),
				!frustum.sphereInFrustum(m_PointLights[i]->Position, m_PointLights[i]->Raduis));
		}
		else
			break;
	}

	// Send Spot Lights informations to shader
	// -----------------------------------------------------------------
	for (unsigned int i = 0; i < MAX_LIGHT_COUNT_SK; i++)
	{
		if (m_SpotLights.empty())
			break;
		if ((m_SpotLights.size() - 1) >= i)
		{
			pbrShader->SetVec3(("sp_lights[" + std::to_string(i) + "].direction").c_str(), m_SpotLights[i]->Direction);
			pbrShader->SetVec3(("sp_lights[" + std::to_string(i) + "].position").c_str(), m_SpotLights[i]->Position);
			pbrShader->SetVec3(("sp_lights[" + std::to_string(i) + "].color").c_str(), m_SpotLights[i]->Color);
			pbrShader->SetFloat(("sp_lights[" + std::to_string(i) + "].intensity").c_str(), m_SpotLights[i]->Intensity);
			pbrShader->SetFloat(("sp_lights[" + std::to_string(i) + "].raduis").c_str(), m_SpotLights[i]->Raduis);
			pbrShader->SetFloat(("sp_lights[" + std::to_string(i) + "].cutOff").c_str(), glm::cos(glm::radians(m_SpotLights[i]->CutOff)));
			pbrShader->SetFloat(("sp_lights[" + std::to_string(i) + "].outerCutOff").c_str(), glm::cos(glm::radians(m_SpotLights[i]->CutOff + m_SpotLights[i]->OuterCutOff)));
			pbrShader->SetFloat(("sp_lights[" + std::to_string(i) + "].cast_shadows").c_str(), m_SpotLights[i]->CastShadows);
			pbrShader->SetFloat(("sp_lights[" + std::to_string(i) + "].Bias").c_str(), m_SpotLights[i]->Bias);
			pbrShader->setInt(("sp_lights[" + std::to_string(i) + "].shadow_index").c_str(), m_SpotLights[i]->shadow_index);
			pbrShader->setInt(("sp_lights[" + std::to_string(i) + "].use").c_str(), m_SpotLights[i]->Active);
			pbrShader->setBool(("sp_lights[" + std::to_string(i) + "].offscreen").c_str(),
				!frustum.sphereInFrustum(m_SpotLights[i]->Position, m_SpotLights[i]->Raduis));
		}
		else
			break;
	}

	// Set Fog config
	// -----------------------------------------------------------------
	pbrShader->setBool("Fog.use", usefog);
	if (usefog)
	{
		pbrShader->SetVec3("Fog.color", fogColor);
		pbrShader->SetFloat("Fog.far", fogFar);
		pbrShader->SetFloat("Fog.near", fogNear);
	}
	// Send PBR environments maps
	// -----------------------------------------------------------------
	BindTexture(0, m_SkyCapture.Irradiance, true);
	BindTexture(1, m_SkyCapture.Prefiltered, true);

	// Send Shadow Maps
	// -----------------------------------------------------------------
	if (m_DirectionalLight != nullptr && m_DirectionalLight->Active && m_DirectionalLight->CastShadows)
	{
		for (size_t i = 0; i < m_ShadowMapper.cascades.size(); i++)
		{
			BindTexture(TEX_SHADOWMAP_1 + i, m_ShadowMapper.cascades[i].depthMap);
		}
	}
	if (!m_PointLights.empty())
	{
		for (size_t i = 0; i < m_PointShadowMapper.shadowMaps.size(); i++)
		{
			BindTexture(TEX_CUBE_SHADOWMAP + i, m_PointShadowMapper.GetShadowMap(i), true);
			if (i == MAX_LIGHT_COUNT_SK)
				break;
		}
	}
	if (!m_SpotLights.empty() && !m_SpotShadowMapper.shadowMaps.empty())
	{
		for (size_t i = 0; i < m_SpotShadowMapper.shadowMaps.size(); i++)
		{
			pbrShader->SetMat4(("spot_MVP[" + std::to_string(i) + "]").c_str(), m_SpotShadowMapper.shadowMaps[i]->MVP);
			BindTexture(TEX_CUBE_SHADOWMAP + 8 + i, m_SpotShadowMapper.GetShadowMap(i));
			if (i == MAX_LIGHT_COUNT_SK)
				break;
		}
	}

	auto anims = m_skMeshs.GetComponents();

	std::string lastMaterial = "";
	for (size_t i = 0; i < anims.size(); i++)
	{
		SkeletalMeshComponent* c_anim = anims[i]; //std::cout << "SkeletalMeshComponent \n";
		if (!c_anim->enabled) continue;
		if (c_anim->mesh == nullptr) continue;
		if (c_anim->materials.empty()) continue;
		
		auto transforms = c_anim->animator.GetFinalBoneMatrices();
		for (size_t i = 0; i < transforms.size(); ++i)
			pbrShader->SetMat4(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), transforms[i]);
		pbrShader->SetMat4("model", glm::scale(c_anim->transform, glm::vec3(c_anim->scale)));

		for (size_t j = 0; j < c_anim->mesh->meshes.size(); j++)
		{
			if (c_anim->isMeshSkiped(j)) continue;
			size_t indx = 0;
			if (j < c_anim->materials.size()) indx = j;
			Material* tmat = c_anim->materials[indx];
			if (lastMaterial != tmat->name)
			{
				pbrShader->SetVec3("albedo", tmat->albedo);
				pbrShader->SetFloat("roughness", tmat->roughness);
				pbrShader->SetFloat("metallic", tmat->metallic);

				pbrShader->SetVec2("tex_uv", tmat->uv);
				// albedo map 
				// -------------------------------------------------------
				if (tmat->tex_albedo != nullptr)
				{
					pbrShader->setBool("use_tex_albedo", true);
					m_cache.BindTexAlbedo(tmat->tex_albedo->getTexID());
				}
				else pbrShader->setBool("use_tex_albedo", false);

				// metalic map 
				// -------------------------------------------------------
				if (tmat->tex_metal != nullptr)
				{
					pbrShader->setBool("use_tex_metal", true);
					m_cache.BindTexMetal(tmat->tex_metal->getTexID());
				}
				else pbrShader->setBool("use_tex_metal", false);
				// roughness map 
				// -------------------------------------------------------
				if (tmat->tex_rough != nullptr)
				{
					pbrShader->setBool("use_tex_rough", true);
					m_cache.BindTexRough(tmat->tex_rough->getTexID());
				}
				else pbrShader->setBool("use_tex_rough", false);

				// normal map 
				// -------------------------------------------------------
				if (tmat->tex_normal != nullptr)
				{
					pbrShader->setBool("use_tex_normal", true);
					m_cache.BindTexNormal(tmat->tex_normal->getTexID());
				}
				else pbrShader->setBool("use_tex_normal", false);

				// Emission 
				// -------------------------------------------------------
				if (tmat->use_emission)
				{
					pbrShader->setBool("use_emission", true);
					pbrShader->SetVec3("emission", tmat->emission);
					pbrShader->SetFloat("emission_power", tmat->emission_power);

					if (tmat->tex_emission != nullptr)
					{
						pbrShader->setBool("use_tex_emission", true);
						m_cache.BindTexEmmi(tmat->tex_emission->getTexID());
					}
					else pbrShader->setBool("use_tex_emission", false);
				}
				else pbrShader->setBool("use_emission", false);

				tmat->setShader(pbrShader);
				pbrShader->SetFloat("ao", tmat->ao * AmbientLevel);

				lastMaterial = tmat->name;
			}

			c_anim->mesh->meshes[j].Draw();
		}
		//c_anim->Draw();
		/*glm::quat q(glm::vec3( 0.f,0.f,0.f));
		glm::mat4 m = mat4_cast(q);
		pbrShader->SetMat4("model", glm::scale(c_anim->transform, glm::vec3(c_anim->scale)) * m);
		c_anim->mesh->meshes[3].Draw();*/
	}
}
// ------------------------------------------------------------------------
void Renderer::UpdateSkeletons(float dt)
{
	m_skMeshs.ClearRemovedComponents();
	auto anims = m_skMeshs.GetComponents();
	for (size_t i = 0; i < anims.size(); i++)
		anims[i]->Update(dt);
}
// ------------------------------------------------------------------------
void Renderer::EndFrame()
{
	if (postProc.blur_use && postProc.Usable()) {
		/* Two pass image bluring */
		glViewport(0, 0, (int)postProc.verticalBlur.scr_w, (int)postProc.verticalBlur.scr_h);
		
		postProc.VerticalBlur(postProc.colorBuffer);
		renderQuad();
		postProc.HorizontalBlur();
		renderQuad();
		
		postProc.VerticalBlur(postProc.BluredScreen);
		renderQuad();
		postProc.HorizontalBlur();
		renderQuad();
	}
	/* Bloom */
	if (postProc.bloom_use && postProc.Usable()) {
		glViewport(0, 0, (int)postProc.bloomTex.scr_w, (int)postProc.bloomTex.scr_h);

		bool horizontal = true, first_iteration = true;

		postProc.bloomTex.Bind();
		postProc.bloomShader.use();
		postProc.bloomShader.SetFloat("threshold", postProc.bloom_threshold);
		glActiveTexture(GL_TEXTURE0);
		postProc.colorBuffer.UseTexture();
		renderQuad();

		postProc.bloomBlurShader.use();
		for (unsigned int i = 0; i < 3; i++)
		{
			postProc.bloomBlurTex.Bind();
			postProc.bloomBlurShader.setBool("horizontal", false);
			postProc.bloomTex.UseTexture();
			renderQuad();

			postProc.bloomTex.Bind();
			postProc.bloomBlurShader.setBool("horizontal", true);
			postProc.bloomBlurTex.UseTexture();
			renderQuad();
		}
	}
	/* Apply image effects */
	/*postProc.ssaoEffect.shaderSSAO->setInt("gDepthMap", 4);
		postProc.ssaoEffect.shaderSSAO->SetMat4("invProj", glm::inverse(cam.GetProjectionMatrix()));
		BindTexture(4, fplus_renderer.depthMap);*/
	/*glBindFramebuffer(GL_FRAMEBUFFER, 0);
	postProc.screenShader.use();
	postProc.screenShader.setInt("gDepthMap", 4);
	postProc.screenShader.SetFloat("Near", MainCam.NearView);
	postProc.screenShader.SetFloat("Far", MainCam.FarView);
	glActiveTexture(GL_TEXTURE4);
	BindTexture(GL_TEXTURE_2D, fplus_renderer.depthMap); */
	/*GLuint tex_id = csm;
	float f, n;
	if (csm == 0) { tex_id = m_ShadowMapper.cascades[0].depthMap; f = m_ShadowMapper.cascades[0].cascadeSplitFar; }
	if (csm == 1) {
		tex_id = m_ShadowMapper.cascades[1].depthMap; f = m_ShadowMapper.cascades[1].cascadeSplitFar;
	}
	if (csm == 2) {
		tex_id = m_ShadowMapper.cascades[2].depthMap; f = m_ShadowMapper.cascades[2].cascadeSplitFar;
	}*/
	if (postProc.Render(0, 0, 0.01f)) {
		glViewport(left_scr_pos, 0, SCR_weight, SCR_height);
		renderQuad();
	}

	// Setup Post Processing buffers if not built or if window resize.
	postProc.SetupBuffer((float)SCR_weight, (float)SCR_height);
}
// ------------------------------------------------------------------------
unsigned int* VecToUInt(vector<unsigned int> &vec)
{
	int size = vec.size();
	unsigned int* lst = new GLuint[size];
	for (int i = 0; i < size; i++)
	{
		lst[i] = vec[i];
	}
	return lst;
}
// ------------------------------------------------------------------------
void Renderer::RenderForLighmap(float* view, float* proj)
{
	glEnable(GL_DEPTH_TEST);

	materials.PbrShader.use();
	glUniformMatrix4fv(materials.PbrShader.GetUniform("projection"), 1, GL_FALSE, proj);
	glUniformMatrix4fv(materials.PbrShader.GetUniform("view"), 1, GL_FALSE, view);

	//--------------- Send Env Cubemaps ----------------//
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_SkyCapture.Irradiance);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_SkyCapture.Prefiltered);

	for (size_t i = 0; i < m_RenderBuffer.OpaqueRenderCommands.size(); i++)
	{
		if (m_RenderBuffer.OpaqueRenderCommands[i].Material == nullptr)
			continue;

		// albedo map -------------------------------------------------------
		if (m_RenderBuffer.OpaqueRenderCommands[i].Material->tex_albedo != nullptr)
		{
			materials.PbrShader.setBool("use_tex_albedo", true);
			glActiveTexture(GL_TEXTURE2);
			m_RenderBuffer.OpaqueRenderCommands[i].Material->tex_albedo->useTexture();
		}
		else
		{
			materials.PbrShader.setBool("use_tex_albedo", false);
		}
		// metalic map -------------------------------------------------------
		materials.PbrShader.setBool("use_tex_metal", false);
		materials.PbrShader.setBool("use_tex_rough", false);
		materials.PbrShader.setBool("use_tex_normal", false);

		materials.PbrShader.SetMat4("model", m_RenderBuffer.OpaqueRenderCommands[i].Transform);
		m_RenderBuffer.OpaqueRenderCommands[i].Material->setShader(&materials.PbrShader);

		m_RenderBuffer.OpaqueRenderCommands[i].Mesh->Draw();
	}
}
// ------------------------------------------------------------------------
Material* Renderer::CreateMaterial(const char* mName)
{
	return materials.CreateMaterial(mName);
}
// ------------------------------------------------------------------------
bool Renderer::SaveMaterial(Material* mat, const char* mPath)
{
	std::cout << "Saving material : " << mPath << std::endl;
	if (mat == nullptr)
		return false;

	YAML::Emitter out;
	mat->serialize<YAML::Emitter>(out);
	RYAML::EndSave(out, mPath);

	return true;
}
// ------------------------------------------------------------------------
bool Renderer::CreateMaterialOnDisk(const char* mPath)
{
	YAML::Emitter out;
	materials.GetMaterialN("")->serialize<YAML::Emitter>(out);
	RYAML::EndSave(out, mPath);

	return true;
}
// ------------------------------------------------------------------------
bool Renderer::CreateMaterialOnDiskWithTexture(std::string& mPath, std::string & TexPath)
{
	YAML::Emitter out;

	Material* mat = materials.GetMaterialN("");
	mat->tex_albedo = resManager->CreateTexture(TexPath, TexPath.c_str());
	mat->serialize<YAML::Emitter>(out);
	mat->tex_albedo = nullptr;

	RYAML::EndSave(out, mPath);

	return true;
}
// ------------------------------------------------------------------------
Material* Renderer::LoadMaterial(const char* mPath)
{
	std::ifstream stream(mPath);
	if (!stream.is_open())
	{
		std::cout << "Unable to load Material : " << mPath << std::endl;
		return materials.GetMaterialN("");
	}

	std::stringstream strStream;
	strStream << stream.rdbuf();
	YAML::Node data;
	data = YAML::Load(strStream.str());

	Material* mat = materials.CreateMaterial(mPath);
	mat->isDefault = false;

	mat->serializeLoad<YAML::Node, ResourcesManager>(data, resManager);
	mat->name = mPath;

	return mat;
}
// ------------------------------------------------------------------------
void Renderer::checkForChanges()
{

}
// ------------------------------------------------------------------------
int Renderer::GetHighlightedEntity(int mx, int my)
{
	glViewport(0, 0, entitySelectionBuffer.scr_w, entitySelectionBuffer.scr_h);

	entitySelectionBuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int val = -1;
	glClearTexImage(entitySelectionBuffer.textureColorbuffer, 0, GL_RED_INTEGER, GL_INT, &val);

	materials.mousePickID.use();
	materials.mousePickID.SetMat4("projection", MainCam.GetProjectionMatrix());
	materials.mousePickID.SetMat4("view", MainCam.GetViewMatrix());

	for (size_t i = 0; i < m_RenderBuffer.OpaqueRenderCommands.size(); i++)
	{
		//std::cout << m_RenderBuffer.OpaqueRenderCommands[i].ID << std::endl;
		materials.mousePickID.setInt("entityID", m_RenderBuffer.OpaqueRenderCommands[i].ID);
		materials.mousePickID.SetMat4("model", m_RenderBuffer.OpaqueRenderCommands[i].Transform);
		m_RenderBuffer.OpaqueRenderCommands[i].Mesh->Draw();
	}
	return entitySelectionBuffer.ReadPixel(0, mx, entitySelectionBuffer.scr_h-my);
}
glm::vec4 Renderer::Get3dPosition(int mx, int my)
{
	glViewport(0, 0, entitySelectionBuffer.scr_w, entitySelectionBuffer.scr_h);

	entitySelectionBuffer.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//int val = -1;
	//glClearTexImage(entitySelectionBuffer.textureColorbuffer, 0, GL_RED_INTEGER, GL_INT, &val);

	materials.mousePickID.use();
	materials.mousePickID.SetMat4("projection", MainCam.GetProjectionMatrix());
	materials.mousePickID.SetMat4("view", MainCam.GetViewMatrix());

	for (size_t i = 0; i < m_RenderBuffer.OpaqueRenderCommands.size(); i++)
	{
		//std::cout << m_RenderBuffer.OpaqueRenderCommands[i].ID << std::endl;
		materials.mousePickID.setInt("entityID", m_RenderBuffer.OpaqueRenderCommands[i].ID);
		materials.mousePickID.SetMat4("model", m_RenderBuffer.OpaqueRenderCommands[i].Transform);
		m_RenderBuffer.OpaqueRenderCommands[i].Mesh->Draw();
	}
	std::vector<Displacement*> Disps = mDisplacements.GetComponents();
	for (size_t i = 0; i < Disps.size(); i++)
	{
		materials.mousePickID.SetMat4("model", Disps[i]->model);
		Disps[i]->Render();
	}

	return entitySelectionBuffer.ReadPixelVec4(0, mx, entitySelectionBuffer.scr_h - my);
}
// ------------------------------------------------------------------------
ReflectionProbe* Renderer::CreateReflectionProbe(EnttID ent_id)
{
	ReflectionProbe* pl = new ReflectionProbe();
	pl->probe_id = ent_id;
	pl->box.BoxMax = glm::vec3(1);
	pl->box.BoxMin = glm::vec3(-1);
	//last_used_id++;
	m_ReflectionProbes.push_back(pl);
	return pl;
}
// ------------------------------------------------------------------------
bool Renderer::RemoveReflectionProbe(EnttID ent_id)
{
	for (size_t i = 0; i < m_ReflectionProbes.size(); i++)
	{
		if (m_ReflectionProbes[i]->probe_id == ent_id)
		{
			ReflectionProbe* pl = m_ReflectionProbes[i];
			m_ReflectionProbes.erase(m_ReflectionProbes.begin() + i);
			delete pl;
			return true;
		}
	}
	return false;
}
// ------------------------------------------------------------------------
ReflectionProbe* Renderer::GetReflectionProbe(EnttID ent_id)
{
	for (size_t i = 0; i < m_ReflectionProbes.size(); i++)
	{
		if (m_ReflectionProbes[i]->probe_id == ent_id)
		{
			return m_ReflectionProbes[i];
		}
	}
	return nullptr;
}
// ------------------------------------------------------------------------
void Renderer::UpdateReflectionProbes()
{
	if (m_ReflectionProbes.empty())
		return;

	for (size_t i = 0; i < m_ReflectionProbes.size(); i++)
	{
		if (m_ReflectionProbes[i]->removed)
		{
			RemoveReflectionProbe(m_ReflectionProbes[i]->entid);
			continue;
		}

		if (!m_ReflectionProbes[i]->baked)
		{
			GLuint cubemap = RenderToCubemap(m_ReflectionProbes[i]->Position
				, m_ReflectionProbes[i]->Resolution
				, 0.01f, 300, m_ReflectionProbes[i]->static_only);
			ibl.CreateCapture(cubemap, m_ReflectionProbes[i]->capture, false);
			glDeleteTextures(1, &cubemap);
			m_ReflectionProbes[i]->baked = true;
		}
	}
}

void Renderer::BakeReflectionProbes()
{
	if (m_ReflectionProbes.empty())
		return;

	for (size_t i = 0; i < m_ReflectionProbes.size(); i++)
	{
		if (m_ReflectionProbes[i]->removed)
		{
			RemoveReflectionProbe(m_ReflectionProbes[i]->entid);
			continue;
		}

		GLuint cubemap = RenderToCubemap(m_ReflectionProbes[i]->Position
			, m_ReflectionProbes[i]->Resolution
			, 0.01f, 300, m_ReflectionProbes[i]->static_only);
		ibl.CreateCapture(cubemap, m_ReflectionProbes[i]->capture, false);
		glDeleteTextures(1, &cubemap);
		m_ReflectionProbes[i]->baked = true;
	}
}
// ------------------------------------------------------------------------
GLuint Renderer::RenderToCubemap(glm::vec3 position, float resolution, float nearPlane, float farPlane, bool static_only)
{
	// Create a Cubemap
	// ----------------------------------------------
	unsigned int textureID;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (unsigned int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, (int)resolution, (int)resolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferCubemap);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureID, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, m_CubemapDepthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (int)resolution, (int)resolution);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_CubemapDepthRBO);

	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		//std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)resolution / (float)resolution, nearPlane, farPlane);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(glm::lookAt(position, position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(glm::lookAt(position, position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(glm::lookAt(position, position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(glm::lookAt(position, position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	// Render scene to cubemap
	// --------------------------------
	glViewport(0, 0, (int)resolution, (int)resolution);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferCubemap);

	Camera capture_cam;
	capture_cam.temp_cam = true;
	capture_cam.projectionMatrix = shadowProj;
	capture_cam.transform.Position = position;
	capture_cam.NearView = nearPlane;
	capture_cam.FarView = farPlane;
	capture_cam.aspectRatio = 1;
	capture_cam.FOV = 45;
	for (size_t i = 0; i < 6; i++)
	{
		capture_cam.viewMatrix = shadowTransforms[i];

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, textureID, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderScene(capture_cam, static_only, m_FramebufferCubemap, (int)resolution);
	}

	return textureID;
}
// ------------------------------------------------------------------------

Texture* Renderer::Get_Lightmap(std::string ppath)
{
	for (size_t i = 0; i < mlightmaps.size(); i++)
	{
		if (mlightmaps[i]->getTexPath() == ppath)
		{
			return mlightmaps[i];
		}
	}

	Texture* texp = new Texture();
	texp->setTexture(ppath.c_str(), ppath, true);
	mlightmaps.push_back(texp);
	return texp;
}
// ------------------------------------------------------------------------
void Renderer::Clear_Lightmaps()
{
	for (size_t i = 0; i < mlightmaps.size(); i++)
	{
		delete mlightmaps[i];
	}
	mlightmaps.clear();
}
// ------------------------------------------------------------------------

#define LIGHTMAPPER_IMPLEMENTATION
//#define LM_DEBUG_INTERPOLATION

#ifndef M_PI // even with _USE_MATH_DEFINES not always available
#define M_PI 3.14159265358979323846
#endif
#include "lightmapper.h"

bool Renderer::BakeSceneLightmaps()
{
	std::cout << "m_LightmapSettings : " << m_LightmapSettings.resolution << " " << m_LightmapSettings.quality << " " << m_LightmapSettings.MaxDistance << "\n";
	m_LightmapSettings.savePath = SceneName + "\\";

	if (m_LightmapSettings.quality != 64 && m_LightmapSettings.quality != 32 &&
		m_LightmapSettings.quality != 16 && m_LightmapSettings.quality != 128)
	{
		std::cout << "Lightmaps Quality Should Be 16, 32, 64, 128 !!" << std::endl;
		return false;
	}

	Clear_Lightmaps();

	for (size_t i = 0; i < m_RenderBuffer.OpaqueRenderCommands.size(); i++)
	{
		if (m_RenderBuffer.OpaqueRenderCommands[i].is_static)
			lm_Count++;
	}

	std::cout << "Baking lightmaps for " << lm_Count << " meshs" << std::endl;
	for (size_t i = 0; i < m_RenderBuffer.OpaqueRenderCommands.size(); i++)
	{
		if (m_RenderBuffer.OpaqueRenderCommands[i].is_static)
			BakeLightMaps(i);
	}
	lm_Count = 0;
	return true;
}

void Renderer::BakeLightMaps(int meshIndex)
{
	int resolution = m_LightmapSettings.resolution;
	int quality = m_LightmapSettings.quality;
	float MaxDistance = m_LightmapSettings.MaxDistance;

	// Initialize Lighmapper
	// -------------------------------------------------------
	lm_context *ctx = lmCreate(
		quality,               // hemicube rendering resolution/quality
		0.001f, MaxDistance,   // zNear, zFar
		0.9f, 0.9f, 0.9f, // sky/clear color
		6, 0.01f          // hierarchical selective interpolation for speedup (passes, threshold)
	);            // modifier for camera-to-surface distance for hemisphere rendering.
				  // tweak this to trade-off between interpolated vertex normal quality and other artifacts (see declaration).
	if (!ctx)
	{
		std::printf("Could not initialize lightmapper.\n");
	}

	int w = resolution, h = resolution;
	float *data = (float*)calloc(w * h * 4, sizeof(float));

	lmSetTargetLightmap(ctx, data, w, h, 4);

	// Bind Meshs
	// -------------------------------------------------------
	int ii = meshIndex;

	const float *pSource;
	float dArray[16] = { 0.0 };

	std::cout << "\n Baking " << ii + 1 << "\\" << lm_Count << std::endl;

	dArray[16] = { 0.0 };

	pSource = (const float*)glm::value_ptr(m_RenderBuffer.OpaqueRenderCommands[ii].Transform);
	for (int j = 0; j < 16; ++j)
		dArray[j] = pSource[j];

	lmSetGeometry(ctx, dArray,
		LM_FLOAT, (unsigned char*)&m_RenderBuffer.OpaqueRenderCommands[ii].Mesh->vertices[0] + offsetof(Vertex, Position), sizeof(Vertex),
		//LM_NONE, NULL, 0,
		LM_FLOAT, (unsigned char*)&m_RenderBuffer.OpaqueRenderCommands[ii].Mesh->vertices[0] + offsetof(Vertex, Normal), sizeof(Vertex),
		LM_FLOAT, (unsigned char*)&m_RenderBuffer.OpaqueRenderCommands[ii].Mesh->vertices[0] + offsetof(Vertex, TexCoords2), sizeof(Vertex),
		m_RenderBuffer.OpaqueRenderCommands[ii].Mesh->indices.size(), LM_UNSIGNED_INT, &m_RenderBuffer.OpaqueRenderCommands[ii].Mesh->indices[0]);

	int vp[4];
	float view[16], projection[16];
	double lastUpdateTime = 0.0;
	while (lmBegin(ctx, vp, view, projection))
	{
		// render to lightmapper framebuffer
		glViewport(vp[0], vp[1], vp[2], vp[3]);

		// --------------------------------------------------------------
		// BEGIN RENDERING SCENE
		// --------------------------------------------------------------
		glEnable(GL_DEPTH_TEST);

		materials.LM.use();
		materials.LM.setInt("u_lightmap", 0);
		materials.LM.SetMat4("u_view", glm::make_mat4(view));
		materials.LM.SetMat4("u_projection", glm::make_mat4(projection));
		glDisable(GL_CULL_FACE);
		for (size_t i = 0; i < m_RenderBuffer.OpaqueRenderCommands.size(); i++)
		{
			if (!m_RenderBuffer.OpaqueRenderCommands[i].is_static) continue;

			materials.LM.SetMat4("u_model", m_RenderBuffer.OpaqueRenderCommands[i].Transform);
			m_RenderBuffer.OpaqueRenderCommands[i].Mesh->Draw();
		}
		glEnable(GL_CULL_FACE);
		// --------------------------------------------------------------
		// END RENDERING SCENE
		// --------------------------------------------------------------

		// display progress every second (printf is expensive)
		double time = glfwGetTime();
		if (time - lastUpdateTime > 1.0)
		{
			lastUpdateTime = time;
			std::printf("\r%6.2f%%", lmProgress(ctx) * 100.0f);
			fflush(stdout);
		}

		lmEnd(ctx);
	}
	//std::printf("\rFinished baking triangles.\n");

	lmDestroy(ctx);

	// postprocess texture
	float *temp = (float*)calloc(w * h * 4, sizeof(float));
	for (int i = 0; i < 16; i++)
	{
		lmImageDilate(data, temp, w, h, 4);
		lmImageDilate(temp, data, w, h, 4);
	}
	lmImageSmooth(data, temp, w, h, 4);
	lmImageDilate(temp, data, w, h, 4);
	lmImagePower(data, w, h, 4, 1.0f);
	std::free(temp);

	std::string texName = "Lightmaps\\";
	texName += m_LightmapSettings.savePath;
	RTools::CreateDir(texName);
	texName += "lightmap";
	texName += std::to_string(ii);
	texName += ".tga";
	// save result to a file
	lmImageSaveTGAf(texName.c_str(), data, w, h, 4, 1.0f);
	std::free(data);

	Get_Lightmap(texName);
}
// ------------------------------------------------------------------------
bool Renderer::isLightBakingSucceed()
{
	return bakingSucceed;
}
// ------------------------------------------------------------------------
void Renderer::SerializeSave(YAML::Emitter& out)
{
	out << YAML::Key << "Renderer" << YAML::Value << YAML::BeginSeq;
	out << YAML::BeginMap;

	out << YAML::Key << "SkyPath" << YAML::Value << SkyPath;
	out << YAML::Key << "UseClouds" << YAML::Value << UseClouds;
	out << YAML::Key << "postProc_Use" << YAML::Value << postProc.Use;
	out << YAML::Key << "usefog" << YAML::Value << usefog;
	out << YAML::Key << "fogNear" << YAML::Value << fogNear;
	out << YAML::Key << "fogFar" << YAML::Value << fogFar;
	out << YAML::Key << "AmbientLevel" << YAML::Value << AmbientLevel;
	out << YAML::Key << "fogColor"; Transform::SerVec3(out, fogColor);
	out << YAML::EndMap;

	out << YAML::BeginMap;

	out << YAML::Key << "ToneMap" << YAML::Value << postProc.ToneMap;

	out << YAML::Key << "vignette_use" << YAML::Value << postProc.vignette_use;
	out << YAML::Key << "vignette_softness" << YAML::Value << postProc.vignette_softness;
	out << YAML::Key << "vignette_radius" << YAML::Value << postProc.vignette_radius;


	out << YAML::Key << "bloom_use" << YAML::Value << postProc.bloom_use;
	out << YAML::Key << "bloom_threshold" << YAML::Value << postProc.bloom_threshold;

	out << YAML::Key << "sharpen" << YAML::Value << postProc.sharpen;
	out << YAML::Key << "sharpen_amount" << YAML::Value << postProc.sharpen_amount;

	out << YAML::Key << "cc_use" << YAML::Value << postProc.cc_use;
	out << YAML::Key << "cc_brightness" << YAML::Value << postProc.cc_brightness;
	out << YAML::Key << "cc_saturation" << YAML::Value << postProc.cc_saturation;
	out << YAML::Key << "cc_contrast" << YAML::Value << postProc.cc_contrast;

	out << YAML::Key << "use_ssao" << YAML::Value << postProc.use_ssao;
	out << YAML::Key << "kernelSize" << YAML::Value << postProc.ssaoEffect.kernelSize;
	out << YAML::Key << "radius" << YAML::Value << postProc.ssaoEffect.radius;
	out << YAML::Key << "ssao_power" << YAML::Value << postProc.ssaoEffect.ssao_power;
	out << YAML::Key << "bias" << YAML::Value << postProc.ssaoEffect.bias;

	out << YAML::Key << "mb_use" << YAML::Value << postProc.mb_use;
	out << YAML::Key << "exposure" << YAML::Value << postProc.exposure;
	out << YAML::EndMap;
	out << YAML::EndSeq;
}
//----------------------------------------------------------------------
void Renderer::SerializeLoad(YAML::Node& out)
{
	auto& rnderSeq = out["Renderer"];
	auto& rnder = rnderSeq[0];
	if (rnder) {
		if(rnder["UseClouds"].IsDefined())
			UseClouds = rnder["UseClouds"].as<bool>();

		SkyPath = rnder["SkyPath"].as<std::string>();
		postProc.Use = rnder["postProc_Use"].as<bool>();
		usefog = rnder["usefog"].as<bool>();
		fogNear = rnder["fogNear"].as<float>();
		fogFar = rnder["fogFar"].as<float>();
		AmbientLevel = rnder["AmbientLevel"].as<float>();
		fogColor = Transform::GetVec3(rnder["fogColor"]);
	}
	auto& postp = rnderSeq[1];
	if (postp)
	{
		if(postp["ToneMap"].IsDefined())
			postProc.ToneMap = postp["ToneMap"].as<int>();

		if (postp["sharpen"].IsDefined())
			postProc.sharpen = postp["sharpen"].as<bool>();
		if (postp["sharpen_amount"].IsDefined())
			postProc.sharpen_amount = postp["sharpen_amount"].as<float>();

		postProc.vignette_use = postp["vignette_use"].as<bool>();
		postProc.vignette_softness = postp["vignette_softness"].as<float>();
		postProc.vignette_radius = postp["vignette_radius"].as<float>();

		postProc.cc_use = postp["cc_use"].as<bool>();
		postProc.cc_brightness = postp["cc_brightness"].as<float>();
		postProc.cc_saturation = postp["cc_saturation"].as<float>();
		postProc.cc_contrast = postp["cc_contrast"].as<float>();

		postProc.bloom_use = postp["bloom_use"].as<bool>();
		postProc.bloom_threshold = postp["bloom_threshold"].as<float>();

		postProc.use_ssao = postp["use_ssao"].as<bool>();
		postProc.ssaoEffect.kernelSize = postp["kernelSize"].as<int>();
		postProc.ssaoEffect.radius = postp["radius"].as<float>();
		postProc.ssaoEffect.ssao_power = postp["ssao_power"].as<float>();
		postProc.ssaoEffect.bias = postp["bias"].as<float>();

		postProc.mb_use = postp["mb_use"].as<bool>();
		postProc.exposure = postp["exposure"].as<float>();
	}
}
