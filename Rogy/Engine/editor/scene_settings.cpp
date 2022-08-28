#include "scene_settings.h"

Scene_Settings::Scene_Settings()
{
}

Scene_Settings::~Scene_Settings()
{
}

void BeginPrepsA(char* nm)
{
	ImGui::Columns(2, nm, false);
	ImGui::SetColumnWidth(0, ImGui::GetWindowWidth()*0.4f);
}

void PrepNameA(char* p_name)
{
	ImGui::Dummy(ImVec2(0, 1.5f));
	ImGui::Text(p_name);
}

void NextPrepsA()
{
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 2);
}

void EndPrepsA()
{
	ImGui::Columns(1);
}

void Scene_Settings::Render()
{
	ImGui::Begin("Scene Settings", NULL, ImGuiWindowFlags_NoCollapse);
	if (rndr == nullptr)
		ImGui::End();

	if (ImGui::CollapsingHeader("SkyBox", ImGuiTreeNodeFlags_DefaultOpen))
	{
		BeginPrepsA("##skypreps");
		PrepNameA("HDR Path");
		PrepNameA("Ambient");
		PrepNameA("Atmosphere Sky");
		PrepNameA("Sky Color");
		PrepNameA("Clouds");
		PrepNameA("Normalized Sun Y");
		PrepNameA("Clouds");
		PrepNameA("Albedo");
		PrepNameA("Turbidity");
		NextPrepsA();
		if (ImGui::Button("Set ##hdr")) 
			ImGuiFileDialog::Instance()->OpenModal("ChooseHDRModel", "Choose Texture", ".hdr\0.HDR\0.png\0.bmp\0.jpg", ".");
		ImGui::SameLine(); ImGui::Text(rndr->SkyPath.c_str());
		ImGui::DragFloat("##Ambientcc", &rndr->AmbientLevel, 0.1f);
		ImGui::Checkbox(" ##UseDynamicSky", &rndr->UseDynamicSky);
		ImGui::ColorEdit3("##skyclor", (float*)&rndr->SkyColor, ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
		ImGui::Checkbox(" ##cloudsss", &rndr->UseClouds);
		ImGui::DragFloat("##m_normalized_sun_y", &rndr->m_normalized_sun_y, 0.01f);
		ImGui::DragFloat("##m_albedo", &rndr->m_albedo, 0.01f);
		ImGui::DragFloat("##m_turbidity", &rndr->m_turbidity, 0.01f);
		float m_normalized_sun_y = 1.15f;
		float m_albedo = 0.1f;
		float m_turbidity = 4.0f;
		EndPrepsA();
		if (ImGui::Button("Update Skybox Ligthing", ImVec2(ImGui::GetWindowWidth(), 0)))
			rndr->BakeGlobalLightData();
		ImGui::Separator();
	}

	if (ImGui::CollapsingHeader("Post Processing", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Enable Post Effects ##Effects", &rndr->postProc.Use);
		ImGui::Text("Tone Mapping"); ImGui::SameLine();
		ImGui::Combo("##tma_type", &rndr->postProc.ToneMap, "FilmicTM\0Reinhard\0HDRTM\0");

		ImGui::Separator();
		ImGui::Text("FXAA"); ImGui::SameLine();
		ImGui::Checkbox("##FXAA", &rndr->postProc.Fxaa);

		ImGui::DragFloat("Exposure ##Exposure", &rndr->postProc.exposure);
		ImGui::Separator();
		ImGui::Checkbox("Vignette ##vignette", &rndr->postProc.vignette_use);
		if (rndr->postProc.vignette_use)
		{
			ImGui::DragFloat("Radius ##vignette", &rndr->postProc.vignette_radius);
			ImGui::DragFloat("Softness ##vignette", &rndr->postProc.vignette_softness);
			ImGui::Separator();
		}
		ImGui::Checkbox("Color Correction ##cc", &rndr->postProc.cc_use);
		if (rndr->postProc.cc_use)
		{	
			ImGui::DragFloat("Brightness ##cc", &rndr->postProc.cc_brightness, 0.01f);
			ImGui::DragFloat("Contrast ##cc", &rndr->postProc.cc_contrast, 0.01f);
			ImGui::DragFloat("Saturation ##cc", &rndr->postProc.cc_saturation, 0.01f);
			ImGui::Separator();
		}
		ImGui::Checkbox("SSAO ##ss", &rndr->postProc.use_ssao);
		if (rndr->postProc.use_ssao)
		{
			ImGui::DragFloat("Power ##ss", &rndr->postProc.ssaoEffect.ssao_power, 0.01f, 0.01f, 2000.0f);
			ImGui::DragInt("Kernel Size ##ss", &rndr->postProc.ssaoEffect.kernelSize, 1, 1, 64);
			ImGui::DragFloat("Radius ##ss", &rndr->postProc.ssaoEffect.radius, 0.01f, 0.01f, 20.0f);
			ImGui::DragFloat("Bias ##ss", &rndr->postProc.ssaoEffect.bias, 0.01f);
			//ImGui::DragFloat("Scale ##ss", &rndr->postProc.ssaoEffect.saoScale);
			//ImGui::DragInt("Turns ##ss", &rndr->postProc.ssaoEffect.saoTurns);
			ImGui::DragInt("Blur Passes ##ss", (int*)&rndr->postProc.ssaoEffect.blurAmount, 1, 0, 10);
			ImGui::Separator();
		}
		ImGui::Checkbox("Bloom ##ss", &rndr->postProc.bloom_use);
		if (rndr->postProc.bloom_use)
		{
			ImGui::DragFloat("Threshold ##ss", &rndr->postProc.bloom_threshold, 0.01f);
			ImGui::Separator();
		}
		ImGui::Checkbox("Blur", &rndr->postProc.blur_use);
		ImGui::Checkbox("Sharpen", &rndr->postProc.sharpen);
		ImGui::DragFloat("Sharpen ##Sharpen", &rndr->postProc.sharpen_amount, 0.1f);
		//ImGui::Checkbox("Camera Motion Blur ##cmb", &rndr->postProc.mb_use);
		ImGui::Separator();
	}

	if (ImGui::CollapsingHeader("Fog", ImGuiTreeNodeFlags_DefaultOpen))
	{
		BeginPrepsA("##skypreps");
		PrepNameA("Enable ");
		PrepNameA("Color ");
		PrepNameA("Near ");
		PrepNameA("Far ");
		NextPrepsA();
		ImGui::Checkbox("##fena", &rndr->usefog);
		ImGui::ColorEdit3("##fenacol", (float*)&rndr->fogColor, ImGuiColorEditFlags_NoInputs);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 2);
		ImGui::DragFloat("##fnear", &rndr->fogNear, 0.01f);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 2);
		ImGui::DragFloat("##ffar", &rndr->fogFar, 0.01f);
		EndPrepsA();
		ImGui::Separator();
	}

	if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen))
	{
		BeginPrepsA("##lmsettingsref");
		PrepNameA("Reflection Bounces");
		NextPrepsA();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 2);
		ImGui::DragInt("##Reflection Bounces", (int*)&rndr->ReflectionBounces, 1, 1, 6);
		EndPrepsA();

		if (ImGui::Button("Bake Reflection Probes", ImVec2(ImGui::GetWindowWidth(), 0)))
			rndr->BakeAllRefProbs();
		if (ImGui::Button("Bake Static Lights", ImVec2(ImGui::GetWindowWidth(), 0)))
			rndr->BakeAllStaticLights();
		//if (ImGui::Button("Bake Nav", ImVec2(ImGui::GetWindowWidth(), 0)))
			//_scene->PushRequest(SR_BAKE_NAV);

		ImGui::Separator();
		ImGui::Text("Lightmapper");
		if(rndr == nullptr)
			ImGui::Text("rndr is null!");
		ImGui::Text("This Lightmapper will bake Ambient Occlusion for All the static meshs \n in the scene.");
		BeginPrepsA("##lmsettings");
		PrepNameA("Resolution ");
		PrepNameA("Quality ");
		PrepNameA("Max Distance ");
		PrepNameA("Passes");
		PrepNameA("Threshold");
		NextPrepsA();
		ImGui::DragInt("##lmres", &rndr->m_LightmapSettings.resolution);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 2);
		ImGui::DragInt("##lmqual", &rndr->m_LightmapSettings.quality);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 2);
		ImGui::DragFloat("##lmmdist", &rndr->m_LightmapSettings.MaxDistance);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 2);
		ImGui::DragInt("##lmmdistPasses", &rndr->m_LightmapSettings.Passes);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 2);
		ImGui::DragFloat("##lmmdistthreshold", &rndr->m_LightmapSettings.threshold);
		EndPrepsA();
		if (ImGui::Button("Bake", ImVec2(ImGui::GetWindowWidth(), 0)) )
		{
			rndr->BakeLighting = true;
		}
		
		ImGui::Separator();
	}
	if (ImGui::CollapsingHeader("Stats", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("%.1f FPS (%.3f ms/frame) ", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("Renderer::MaterialChanges ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(rndr->MaterialChanges).c_str());
		ImGui::Text("Renderer::DrawCalls ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(rndr->DrawCalls).c_str());
		ImGui::Text("Renderer::DrawCallsCSM ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(rndr->DrawCallsCSM).c_str());
		ImGui::Text("Renderer::DrawCallsPointShadows ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(rndr->DrawCallsPointShadows).c_str());
		
		ImGui::Separator();
	}

	ImGui::End();

	if (ImGuiFileDialog::Instance()->FileDialog("ChooseHDRModel"))
	{
		if (ImGuiFileDialog::Instance()->IsOk == true)
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
			rndr->new_skyPath = filePathName;
			rndr->skyTexChange = true;
		}
		ImGuiFileDialog::Instance()->CloseDialog("ChooseHDRModel");
	}
}

