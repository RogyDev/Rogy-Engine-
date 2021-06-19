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
		NextPrepsA();
		if (ImGui::Button("Set ##hdr")) 
			ImGuiFileDialog::Instance()->OpenModal("ChooseHDRModel", "Choose Texture", ".hdr\0.HDR\0.png\0.bmp\0", ".");
		ImGui::SameLine(); ImGui::Text(rndr->SkyPath.c_str());
		ImGui::DragFloat("##Ambientcc", &rndr->AmbientLevel);
		EndPrepsA();
		ImGui::Separator();
	}

	if (ImGui::CollapsingHeader("Post Processing", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Enable Post Effects ##Effects", &rndr->postProc.Use);
		ImGui::Separator();
		ImGui::DragFloat("Exposure ##Exposure", &rndr->postProc.exposure);
		//ImGui::DragFloat("Exposure Speed ##Exposure", &rndr->exposure_speed);
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
			ImGui::DragFloat("Power ##ss", &rndr->postProc.ssaoEffect.ssao_power, 0.01f, 0.01f, 1.0f);
			ImGui::DragInt("Kernel Size ##ss", &rndr->postProc.ssaoEffect.kernelSize, 1, 1, 64);
			ImGui::DragFloat("Radius ##ss", &rndr->postProc.ssaoEffect.radius, 0.01f, 0.01f, 20.0f);
			ImGui::DragFloat("Bias ##ss", &rndr->postProc.ssaoEffect.bias, 0.01f);
			ImGui::Separator();
		}
		ImGui::Checkbox("Blur", &rndr->postProc.blur_use);
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
		if (ImGui::Button("Bake Reflection Probes", ImVec2(ImGui::GetWindowWidth(), 0)))
			rndr->BakeReflectionProbes();
		if (ImGui::Button("Bake Static Lights", ImVec2(ImGui::GetWindowWidth(), 0)))
			rndr->BakeReflectionProbes();
		ImGui::Separator();
		ImGui::Text("Lightmapper");
		ImGui::Text("This Lightmapper will bake Ambient Occlusion for All the static meshs \n in the scene.");
		BeginPrepsA("##lmsettings");
		PrepNameA("Resolution ");
		PrepNameA("Quality ");
		PrepNameA("Max Distance ");
		NextPrepsA();
		ImGui::DragInt("##lmres", &rndr->m_LightmapSettings.resolution);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 2);
		ImGui::DragInt("##lmqual", &rndr->m_LightmapSettings.quality);
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 2);
		ImGui::DragFloat("##lmmdist", &rndr->m_LightmapSettings.MaxDistance);
		EndPrepsA();
		if (ImGui::Button("Bake", ImVec2(ImGui::GetWindowWidth(), 0)) )
		{
			rndr->BakeLighting = true;
		}
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

