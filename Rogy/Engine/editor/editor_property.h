#ifndef EDITOR_PROPERTIES_H
#define EDITOR_PROPERTIES_H

//#include "../resources/resources_manger.h"

#include "../physics/PhysicsWorld.h"
#include "../audio/AudioManager.h"
#include "../ui/UIRenderer.h"
#include "../renderer/renderer.h"

#include "scene_hierarchy.h"
#include "material_editor.h"

#include "../nativeScripting/ScriptableEntity.h"
#include "../scripting/ScriptManager.h"



#define WIN32

class EditorProperty
{
public:
	EditorProperty();
	~EditorProperty();

	ResourcesManager* res = nullptr;
	Scene_hierarchy* nodes = nullptr;
	Renderer* rndr = nullptr;
	PhysicsWorld* phy_world = nullptr;
	ScriptManager* scrMnger = nullptr;
	AudioManager* audio_mnger = nullptr;
	UIRenderer* ui_renderer = nullptr;
	RMaterialEditor* mat_editor = nullptr;

	bool showMat = false;

	static void BeginPreps(const char* nm);
	static void PrepName(const char* p_name);
	static void NextPreps();
	static void EndPreps();
	static std::string GetRelativePath(std::string target);
	static void SetWeightPrep();
	static void DisplayInfo(const char* desc);

	void EditTextureProprty(Material* tex);

	void point_light_editor(Entity& obj);

	void billboard_editor(Entity& obj);

	void general_editor(Entity &obj);
	void mesh_editor(Entity &obj);
	
	void spot_light_editor(Entity &obj);
	void dir_light_editor(Entity &obj);
	void ref_probe_editor(Entity &obj);
	void rb_editor(Entity &obj);
	void scriptC_editor(Entity &obj, ScriptInstance* script);
	void particles_editor(Entity &obj);
	void audio_editor(Entity &obj);
	void grass_editor(Entity &obj);
	void displacement_editor(Entity &obj);
	void cpp_script_editor(Entity &obj);
	void terrainCollider_editor(Entity &obj);

	void cam_editor(Entity &obj);

	void animation_editor(Entity &obj);

	void UIwidget_editor(Entity &obj);

	void components_editor(Entity &obj);

	void CreateEntityWithMesh(std::string& ent_name, std::string& mesh_path);

	void Render();
	template<typename T>
	void material_editor(T* rc)
	{
		if (!rc || rc->material == nullptr)
			return;

		bool ch = ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen);

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RES_FILE_MAT", ImGuiDragDropFlags_SourceAllowNullID))
			{
				//IM_ASSERT(payload->DataSize == sizeof(std::string));
				//std::string payload_n = std::string((const char*)payload->Data);
				//std::count << "MAT : " << (const char*)payload->Data << std::endl;
				const wchar_t* ppp = (const wchar_t*)payload->Data;
				std::string payload_n((const char*)ppp);
				rc->material = rndr->LoadMaterial(payload_n.c_str());

				int slashPos = 0;
				for (size_t j = payload_n.length() - 1; j > 0; j--)
				{
					if (payload_n[j] == '\\')
					{
						slashPos = j;
						break;
					}
				}
				rc->material->path = payload_n.substr(slashPos + 1, std::string::npos);
			}
			ImGui::EndDragDropTarget();
		}

		if(ch)
		{
			if (rc->material->isDefault)
			{
				ImGui::Text("Source"); ImGui::SameLine(0, ImGui::GetWindowWidth()*0.2f);

				ImGui::Button("Default-Material");
					
				if (ImGui::IsItemClicked(1))
				{
					ImGui::OpenPopup("mat_edit_menu");
				}
				if (ImGui::IsItemClicked(0))
				{
					if (rc->material != nullptr && !rc->material->isDefault)
						mat_editor->EditMaterial(rc->material);
				}
			}
			else
			{
				ImGui::Text("Source"); ImGui::SameLine(0, ImGui::GetWindowWidth()*0.2f);
				ImGui::Button(rc->material->getMatPath().c_str());
				if (ImGui::IsItemClicked(1))
				{
					ImGui::OpenPopup("mat_edit_menu");
				}
				if (ImGui::IsItemClicked(0))
				{
					if (rc->material != nullptr && !rc->material->isDefault)
						mat_editor->EditMaterial(rc->material);
				}
				DisplayInfo(rc->material->getMatName().c_str());

				/*if (showMat && ImGui::Button("Collapse", ImVec2(ImGui::GetWindowWidth(), 0)))
					showMat = false;
				else if (!showMat && ImGui::Button("Edit", ImVec2(ImGui::GetWindowWidth(), 0)))
					showMat = true;

				if (showMat)
				{
					if (rc->material->type == SHADER_PBR)
					{
						if (ImGui::Button("Save", ImVec2(ImGui::GetWindowWidth(), 0)))
							rndr->SaveMaterial(rc->material, rc->material->getMatName().c_str());
						BeginPreps("edit_mat34");
						PrepName("TAG");
						PrepName("Color");
						PrepName("Metallic");
						PrepName("Roughness");
						PrepName("AO");
						PrepName("UV");

						PrepName("Emission");
						if (rc->material->use_emission)
						{
							PrepName("Emission Color");
							PrepName("Emission Power");
						}
						PrepName("Albedo Texture");
						ImGui::Dummy(ImVec2(0, 30.0f));
						PrepName("Normal Map Texture");
						ImGui::Dummy(ImVec2(0, 30.0f));
						PrepName("Metallic Texture");
						ImGui::Dummy(ImVec2(0, 30.0f));
						PrepName("Roughness Texture");
						ImGui::Dummy(ImVec2(0, 30.0f));
						PrepName("Emission Texture");

						NextPreps();
						SetWeightPrep(); ImGui::InputText("##mat_tag", &rc->material->tag);
						ImGui::ColorEdit3("##AlbedoMat", (float*)&rc->material->albedo, ImGuiColorEditFlags_NoInputs);
						SetWeightPrep(); ImGui::DragFloat("##MetallicMat", &rc->material->metallic, 0.03f, 0.0f, 1.0f);
						SetWeightPrep(); ImGui::DragFloat("##RoughnessMat", &rc->material->roughness, 0.03f, 0.0f, 1.0f);
						SetWeightPrep(); ImGui::DragFloat("##AONat", &rc->material->ao, 0.01f, 0, 1, "%.3g");
						SetWeightPrep(); ImGui::DragFloat2("##uvNat", (float*)&rc->material->uv, 0.01f);

						ImGui::Checkbox("##emmUsage", &rc->material->use_emission);
						if (rc->material->use_emission)
						{
							SetWeightPrep();
							ImGui::ColorEdit3("##emmMat", (float*)&rc->material->emission, ImGuiColorEditFlags_NoInputs);
							SetWeightPrep();
							ImGui::SliderFloat("##emmpMat", &rc->material->emission_power, 0, 100, "%.3g");
						}
						EditTextureProprty(rc->material);
						EndPreps();
					}
				}*/
			}
			ImGui::Separator();

			if (ImGui::BeginPopup("mat_edit_menu"))
			{
				if (ImGui::Selectable("Default-Material"))
					rc->material = rndr->materials.GetMaterialN("");
				if (ImGui::Selectable("New"))
					ImGuiFileDialog::Instance()->OpenModal("CreateMatModel", "New Material", ".mat\0.mat\0.mat\0\0", ".mat");
				if (ImGui::Selectable("Load"))
					ImGuiFileDialog::Instance()->OpenModal("ChooseMatModel", "Select Material", ".mat\0.mat\0.mat\0\0", ".");
				if (ImGui::Selectable("Edit"))
					mat_editor->EditMaterial(rc->material);
				ImGui::EndPopup();
			}

			if (ImGuiFileDialog::Instance()->FileDialog("ChooseMatModel"))
			{
				// action if OK
				if (ImGuiFileDialog::Instance()->IsOk == true)
				{
					std::string filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
					std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

					RGetRelativePath(filePathName);
					RGetRelativePath(filePath);

					std::cout << "loading material : " << filePathName.c_str() << std::endl;
					// action
					rc->material = rndr->LoadMaterial(filePathName.c_str());

					filePath += "\\";
					eraseSubStr(filePathName, filePath);
					rc->material->path = filePathName;
				}
				// close
				ImGuiFileDialog::Instance()->CloseDialog("ChooseMatModel");
			}

			if (ImGuiFileDialog::Instance()->FileDialog("CreateMatModel"))
			{
				// action if OK
				if (ImGuiFileDialog::Instance()->IsOk == true)
				{
					std::string filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
					std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

					RGetRelativePath(filePathName);
					RGetRelativePath(filePath);

					// action
					rndr->CreateMaterialOnDisk(filePathName.c_str());
					rc->material = rndr->LoadMaterial(filePathName.c_str());

					filePath += "\\";
					eraseSubStr(filePathName, filePath);
					rc->material->path = filePathName;
				}
				// close
				ImGuiFileDialog::Instance()->CloseDialog("CreateMatModel");
			}
		}
	}
private:
	EnttID save_ent_id = -1;
	//std::string scrfilter;
	template<typename T, typename UIFunction>
	bool DrawComponent(const std::string& name, Entity* entity, UIFunction uiFunction);
};

#endif