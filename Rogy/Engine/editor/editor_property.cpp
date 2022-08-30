#include "editor_property.h"
// -----------------------------------------------------------------------
EditorProperty::EditorProperty()
{
}
// -----------------------------------------------------------------------
EditorProperty::~EditorProperty()
{
}
// -----------------------------------------------------------------------
void EditorProperty::BeginPreps(const char* nm)
{
	ImGui::Columns(2, nm, false);
	ImGui::SetColumnWidth(0, ImGui::GetWindowWidth()*0.4f);
}
// -----------------------------------------------------------------------
void EditorProperty::PrepName(const char* p_name)
{
	ImGui::Dummy(ImVec2(0, 2.0f));
	ImGui::Text(p_name);
}
// -----------------------------------------------------------------------
void EditorProperty::NextPreps()
{
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 2);
}
// -----------------------------------------------------------------------
void EditorProperty::DisplayInfo(const char* desc)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}
// -----------------------------------------------------------------------
void EditorProperty::EndPreps()
{
	ImGui::Columns(1);
}
// -----------------------------------------------------------------------
template<typename T, typename UIFunction>
bool EditorProperty::DrawComponent(const std::string& name, Entity* entity, UIFunction uiFunction)
{
	if (entity->HasComponent<T>()) {
		
		bool is_down = ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
		std::string popupTag = name; popupTag += "ComponentSettings";
		if (ImGui::IsItemClicked(1))
		{
			ImGui::OpenPopup(popupTag.c_str());
		}

		bool removeComponent = false;
		if (ImGui::BeginPopup(popupTag.c_str()))
		{
			if (ImGui::MenuItem("Remove component"))
				removeComponent = true;

			ImGui::EndPopup();
		}

		if (is_down)
		{
			T* component = entity->GetComponent<T>();
			uiFunction(entity, component, removeComponent);
			ImGui::Separator();
		}
		if (removeComponent)
			return true;
	}
	return false;
}
// -----------------------------------------------------------------------
std::string EditorProperty::GetRelativePath(std::string target)
{
	std::string rp = target;
	eraseSubStr(rp, ImGuiFileDialog::Instance()->GetCurrentPath());
	return rp;
}
// -----------------------------------------------------------------------
void EditVec3_xyz_G(const char* name_id, glm::vec3 &values)
{
	float siz = (ImGui::GetWindowWidth() / 2) / 3;
	string col_s("##1");
	col_s += name_id;
	ImGui::SetNextItemWidth(siz);
	ImGui::DragFloat(col_s.c_str(), &values.x, 0.1f, 0, 0, "X: %.3g"); ImGui::SameLine();
	ImGui::SetNextItemWidth(siz);
	col_s += "2";
	ImGui::DragFloat(col_s.c_str(), &values.y, 0.1f, 0, 0, "Y: %.3g"); ImGui::SameLine();
	ImGui::SetNextItemWidth(siz);
	col_s += "3";
	ImGui::DragFloat(col_s.c_str(), &values.z, 0.1f, 0, 0, "Z: %.3g");
	col_s.clear();
}
// -----------------------------------------------------------------------
void EditVec3_xyz(const char* name_id, glm::vec3 &values)
{
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 1.8f);
	ImGui::DragFloat3(name_id, (float*)&values, 0.1f, 0, 0, "%.1f");
}
// -----------------------------------------------------------------------
void EditorProperty::SetWeightPrep()
{
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 1.8f);
}
// -----------------------------------------------------------------------
void EditorProperty::general_editor(Entity &obj)
{
	if (ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
	{
		BeginPreps("editTr");
		PrepName("Active");
		PrepName("Name");
		PrepName("Static");
		PrepName("Tag");

		NextPreps();
		ImGui::Checkbox("##active", &obj.Active);
		SetWeightPrep(); ImGui::InputText("##change_name", &obj.name);
		ImGui::Checkbox("##static ", &obj.Static);
		SetWeightPrep(); ImGui::InputText("##change_tag", &obj.tag);

		EndPreps();
		//ImGui::Text("Child count = %d" , obj.ChildCount());

		//ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.45f, 0.5f, 1));
		if (ImGui::Button("Add Component", ImVec2(ImGui::GetWindowWidth(), 0)))
			ImGui::OpenPopup("add_com");
	//	ImGui::PopStyleColor();

		if (ImGui::BeginPopup("add_com"))
		{
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 0.7f), "--------------------- Components ---------------------");
			// # Mesh  ---------------------------------------------------------------------------
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 0.7f), "# Mesh");
			if (ImGui::Selectable("Mesh Renderer"))
			{
				if (!obj.HasComponent<RendererComponent>()) {
					RendererComponent* rc = rndr->m_renderers.AddComponent(obj.ID);
					rc->mesh = nullptr;
					rc->material = rndr->CreateMaterial("");
					obj.AddComponent<RendererComponent>(rc);
				}
			}

			if (ImGui::Selectable("Billboard"))
			{
				if (!obj.HasComponent<BillboardComponent>()) {
					BillboardComponent* bb = rndr->CreateBillboard(obj.ID);
					bb->tex_path = "core\\textures\\glow1.png";
					obj.AddComponent<BillboardComponent>(bb);
				}
			}
			if (ImGui::Selectable("Particle System"))
			{
				if (!obj.HasComponent<ParticleSystem>()) {
					ParticleSystem* bb = rndr->mParticals.AddComponent(obj.ID);
					bb->mTexture = res->CreateTexture("core\\textures\\glow1.png", "core\\textures\\glow1.png");
					obj.AddComponent<ParticleSystem>(bb);
				}
			}
			if (ImGui::Selectable("Grass Component"))
			{
				if(!obj.HasComponent<GrassComponent>())
					obj.AddComponent<GrassComponent>(rndr->mGrass.AddComponent(obj.ID));
			}
			if (ImGui::Selectable("Skeletal Mesh"))
			{
				if (!obj.HasComponent<SkeletalMeshComponent>()) {
					SkeletalMeshComponent* rc = rndr->m_skMeshs.AddComponent(obj.ID);
					rc->mesh = nullptr;
					rc->material = rndr->CreateMaterial("");
					rc->materials.push_back(rc->material);
					obj.AddComponent<SkeletalMeshComponent>(rc);
				}
			}
			ImGui::Separator();
			// # Scenes  ---------------------------------------------------------------------------
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 0.7f), "# Rendering");
			if (ImGui::Selectable("Camera"))
				obj.AddComponent<CameraComponent>(rndr->m_cameras.AddComponent(obj.ID));
			ImGui::Separator();
			// # Lighting  ---------------------------------------------------------------------------
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 0.7f), "# Lighting");
			if (ImGui::Selectable("Directional Light"))
			{
				if (!obj.HasComponent<DirectionalLight>())
					obj.AddComponent<DirectionalLight>(rndr->CreateDirectionalLight());
			}
			if (ImGui::Selectable("Point Light"))
			{
				if (!obj.HasComponent<PointLight>())
					obj.AddComponent<PointLight>(rndr->CreatePointLight(obj.ID));
			}
			if (ImGui::Selectable("Spot Light"))
			{
				if (!obj.HasComponent<SpotLight>())
					obj.AddComponent<SpotLight>(rndr->CreateSpotLight(obj.ID));
			}
			if (ImGui::Selectable("Reflection Probe"))
			{
				if (!obj.HasComponent<ReflectionProbe>())
					obj.AddComponent<ReflectionProbe>(rndr->CreateReflectionProbe(obj.ID));
			}
			ImGui::Separator();
			// # Physics  ---------------------------------------------------------------------------
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 0.7f), "# Physics ");
			if (ImGui::Selectable("Rigid Body"))
			{
				if (phy_world != nullptr)
				{
					if (obj.Static)
						phy_world->AddRigidBody(&obj, 0.0f);
					else
						phy_world->AddRigidBody(&obj);
				}
			}
			ImGui::Separator();
			// # Audio  ---------------------------------------------------------------------------
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 0.7f), "# Audio ");
			if (ImGui::Selectable("Audio Source"))
			{
				if (audio_mnger != nullptr)
				{
					obj.AddComponent<RAudioSource>(audio_mnger->AddComponent(obj.ID));
				}
			}
			ImGui::Separator();
			// # UI  ---------------------------------------------------------------------------
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 0.7f), "# UI");
			if (ImGui::Selectable("UI Widget"))
				obj.AddComponent<UIWidget>(ui_renderer->uiWidgets.AddComponent(obj.ID));
			ImGui::Separator();
			// # Scripts  ---------------------------------------------------------------------------
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 0.7f), "# Scripts");
			static std::string scrfilter;
			ImGui::SetNextItemWidth(270.0f);
			ImGui::InputTextWithHint("##_scrfilter", "Search...", &scrfilter);
			ImGui::BeginChild("Scripts", ImVec2(270.0f, 100.0f));
			for (size_t i = 0; i < scrMnger->m_loaded_scripts.size(); i++)
			{
				const char* className = scrMnger->m_loaded_scripts[i].class_name.c_str();
				bool startswith = true;
				if (!scrfilter.empty())
				{
					for (size_t i = 0; i < scrfilter.size(); i++)
					{
						if (scrfilter[i] != className[i])
						{
							startswith = false;
							break;
						}
					}
				}
				if (startswith && scrMnger->m_loaded_scripts[i].valide && ImGui::Selectable(className))
				{
					obj.AddScript(scrMnger->InstanceComponentClass(obj.ID, className));
				}
			}
			ImGui::EndChild();
			ImGui::EndPopup();
		}
		ImGui::Separator();
	}

	if (ImGuiFileDialog::Instance()->FileDialog("ChooseFileModel"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk == true)
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
			RGetRelativePath(filePathName);
			// action
			if (obj.HasComponent<RendererComponent>()) 
				obj.GetComponent<RendererComponent>()->mesh = res->mMeshs.CreateModel(filePathName)->GetFirstMesh();
		}
		// close
		ImGuiFileDialog::Instance()->CloseDialog("ChooseFileModel");
	}

	if (ImGuiFileDialog::Instance()->FileDialog("ChooseFileModelsk"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk == true)
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
			RGetRelativePath(filePathName);
			// action
			if (obj.HasComponent<SkeletalMeshComponent>()) {
				SkeletalMeshComponent* smc = obj.GetComponent<SkeletalMeshComponent>();
				smc->mesh = res->mMeshs.LoadSkeletalModel(filePathName);
				if (smc->mesh != nullptr) {
					for (size_t i = 0; i < smc->mesh->num_anims; i++)
					{
						std::string anim_nn = filePathName;
						anim_nn += to_string(i);
						//smc->LoadAnimation(anim_nn, filePathName, i);
						Animation* anim = res->GetAnimation(anim_nn, filePathName, smc->mesh, i);
						if (anim != nullptr)
							smc->AddAnimation(anim);
					}
				}
			}
			/*if (obj.HasComponent<SkeletalMeshComponent>())
				obj.GetComponent<SkeletalMeshComponent>()->mesh = res->mMeshs.LoadSkeletalModel(filePathName);*/
		}
		// close
		ImGuiFileDialog::Instance()->CloseDialog("ChooseFileModelsk");
	}

	if (ImGuiFileDialog::Instance()->FileDialog("ChooseFileModelsk_anim"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk == true)
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
			RGetRelativePath(filePathName);

			SkeletalMeshComponent* skAnim = obj.GetComponent<SkeletalMeshComponent>();
			// action
			if (skAnim != nullptr) 
			{
				Animation* anim = res->GetAnimation(filePathName, filePathName, skAnim->mesh);
				if (anim != nullptr)
					skAnim->AddAnimation(anim);
			}
		}
		// close
		ImGuiFileDialog::Instance()->CloseDialog("ChooseFileModelsk_anim");
	}
}
// -----------------------------------------------------------------------
void EditorProperty::billboard_editor(Entity& obj)
{
	bool removeComponent = DrawComponent<BillboardComponent>("Billboard", &obj,
		// Draw Properties
		[](Entity* ent, BillboardComponent* bb, bool removeComponent)
	{
		BeginPreps("edit_nnbbd");
		PrepName("Enabled");
		PrepName("Texture");
		PrepName("Color");
		PrepName("Size");
		PrepName("Depth test");
		PrepName("Sky source");
		PrepName("Use Texture for mask only");
		NextPreps();
		SetWeightPrep(); ImGui::Checkbox("##bbroad_active", &bb->enabled);

		if (ImGui::Button("Set ##bbnor"))
			ImGuiFileDialog::Instance()->OpenModal("ChooseTexBBModel", "Choose Texture", ".png\0.bmp\0.tga\0.PNG\0.BMP\0.TGA", ".");
		ImGui::SameLine(); ImGui::Text(bb->tex_path.c_str());
		ImGui::ColorEdit3("##mmbbcol", (float*)&bb->color, ImGuiColorEditFlags_NoInputs);
		SetWeightPrep(); ImGui::DragFloat2("##bbroad_size_edit", (float*)&bb->size);
		ImGui::Checkbox("##bbroad_dtest", &bb->depth_test);
		ImGui::Checkbox("##bbroad_sssource", &bb->sun_source);
		ImGui::Checkbox("##bbroad_sssourcemask", &bb->use_tex_as_mask);
		EndPreps();

		if (ImGuiFileDialog::Instance()->FileDialog("ChooseTexBBModel"))
		{
			// action if OK
			if (ImGuiFileDialog::Instance()->IsOk == true)
			{
				bb->tex_path = ImGuiFileDialog::Instance()->GetFilepathName();
			}
			// close
			ImGuiFileDialog::Instance()->CloseDialog("ChooseTexBBModel");
		}
	});
	
	if (removeComponent)
	{
		obj.RemoveComponent<BillboardComponent>();
	}
}
// -----------------------------------------------------------------------
void EditorProperty::scriptC_editor(Entity &obj, ScriptInstance* script)
{
	if (script != nullptr)
	{
		bool is_on = ImGui::CollapsingHeader((script->class_name + "  (Script)").c_str(), ImGuiTreeNodeFlags_DefaultOpen);

		std::string popupTag = script->class_name; popupTag += "ScriptComponentSettings";

		if (ImGui::IsItemClicked(1))
			ImGui::OpenPopup(popupTag.c_str());

		if (is_on)
		{
			BeginPreps((script->class_name + "edit_scr").c_str());

			for (size_t i = 0; i < script->properties.size(); i++)
			{
				PrepName(script->properties[i].name.c_str());
			}

			NextPreps();
			//try{
				for (size_t i = 0; i < script->properties.size(); i++)
				{
					const char* var_name = script->properties[i].name.c_str();
					ImGui::PushID(var_name + i);
					//std::string hide = "##";
					if (script->properties[i].type == VAR_Int)
					{
						int var = script->GetVarInt(var_name);
						int temp_var = var;
						SetWeightPrep();
						//ImGui::DragInt(std::string(hide + script->class_name + var_name).c_str(), &var);
						ImGui::DragInt("##var", &var);
						if (var != temp_var)
							script->SetVar(var_name, var);
					}
					else if (script->properties[i].type == VAR_Float)
					{
						float var = script->GetVarFloat(var_name);
						float temp_var = var;
						SetWeightPrep();
						//ImGui::DragFloat(std::string(hide + script->class_name + var_name).c_str(), &var, 0.1f);
						ImGui::DragFloat("##var", &var, 0.1f);
						if (var != temp_var)
							script->SetVar(var_name, var);
					}
					else if (script->properties[i].type == VAR_String)
					{
						std::string var = script->GetVarString(var_name);
						std::string temp_var = var;
						SetWeightPrep();
						//ImGui::InputText(std::string(hide + script->class_name + var_name).c_str(), &var);
						ImGui::InputText("##var", &var);
						if (var != temp_var)
							script->SetVar(var_name, var);
					}
					else if (script->properties[i].type == VAR_Bool)
					{
						bool var = script->GetVarBool(var_name);
						bool temp_var = var;
						//ImGui::Checkbox(std::string(hide + script->class_name + var_name).c_str(), &var);
						ImGui::Checkbox("##var", &var);
						if (var != temp_var)
							script->SetVar(var_name, var);
					}
					else if (script->properties[i].type == VAR_VEC3)
					{
						glm::vec3 scr_vec = script->GetVec3(script->properties[i].name);
						glm::vec3 old_vec = scr_vec;
						//EditVec3_xyz(("##scr_vec" + script->properties[i].name).c_str(), scr_vec);
						ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 1.8f);
						ImGui::DragFloat3("##scr_vec", (float*)&scr_vec, 0.1f);
						if (scr_vec != old_vec)
							script->SetVec3(script->properties[i].name, scr_vec);
					}
					else if (script->properties[i].type == VAR_ASSET)
					{
						std::string& asset_path = script->GetAssetPath(script->properties[i].name);
						//asset_path += "##";
						asset_path += " (Asset)";
						//ImGui::InputText(std::string(hide + script->class_name + var_name).c_str(), &asset_path);
						SetWeightPrep();
						ImGui::InputText("##var", &asset_path);
						//ImGui::Button((asset_path + script->properties[i].name).c_str());
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RES_FILE_RBP"))
							{
								IM_ASSERT(payload->DataSize == sizeof(std::string));
								std::string payload_n = *(std::string*)payload->Data;
								script->SetAssetPath(script->properties[i].name, payload_n);
								std::cout << "Get prefab in : " << payload_n << std::endl;
							}
							ImGui::EndDragDropTarget();
						}
					}
					ImGui::PopID();
				}
				//ImGui::PopID();
			//}
			//catch (std::exception e) { std::cout << "lua exeption \n"; }
			EndPreps();
			ImGui::Separator();
		}

		if (ImGui::BeginPopup(popupTag.c_str()))
		{
			if (ImGui::MenuItem("Remove component"))
			{
				obj.RemoveScript(script->class_name);
			}

			ImGui::EndPopup();
		}
	}
}
// -----------------------------------------------------------------------
static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 107.0f)
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2, 0, false);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	//float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	//ImVec2 buttonSizes = { lineHeight + 3.0f, lineHeight };
	ImVec2 buttonSize = { 0.0f , 0.0f };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);
	ImGui::Button("X", buttonSize);
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.15f, 0.45f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);
	ImGui::Button("Y", buttonSize);
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.08f, 0.22f, 0.65f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);
	ImGui::Button("Z", buttonSize);
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}
// -----------------------------------------------------------------------
void transform_editor(Entity &obj)
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		/*EditorProperty::BeginPreps("editTr");
		EditorProperty::PrepName("Position");
		EditorProperty::PrepName("Rotation");
		EditorProperty::PrepName("Scale");
		EditorProperty::NextPreps();

		glm::vec3 get_pos = obj.transform.GetLocalPosition();
		//EditVec3_xyz("##pos", get_pos);
		DrawVec3Control("##pos", get_pos);
		obj.SetTranslation(get_pos, true);
		
		glm::vec3 get_ang = obj.transform.GetEurlerAngels();
		//EditVec3_xyz("##rot", get_ang);
		DrawVec3Control("##rot", get_ang);
		//obj.transform.SetAngels(get_ang);
		obj.SetRotation(get_ang);

		glm::vec3 get_sca = obj.transform.GetLocalScale();
		//EditVec3_xyz("##sca", get_sca);
		DrawVec3Control("##sca", get_sca);
		obj.SetScale(get_sca);

		EditorProperty::EndPreps();*/

		glm::vec3 get_pos = obj.transform.GetLocalPosition();
		DrawVec3Control("Position", get_pos);
		obj.SetTranslation(get_pos, true);

		glm::vec3 get_ang = obj.transform.GetEurlerAngels();
		DrawVec3Control("Rotation", get_ang);
		obj.SetRotation(get_ang);

		glm::vec3 get_sca = obj.transform.GetLocalScale();
		DrawVec3Control("Scale", get_sca);
		obj.SetScale(get_sca);

		ImGui::Separator();
	}
}
// -----------------------------------------------------------------------
void EditorProperty::mesh_editor(Entity &obj)
{
	bool remove = DrawComponent<RendererComponent>("Mesh Renderer", &obj,
		// Draw Properties
		[](Entity* ent, RendererComponent* rc, bool removeComponent)
	{
		BeginPreps("edit_mesh");
		PrepName("Source");
		PrepName("Cast Shadows");
		NextPreps();
		bool hasMesh = (rc->mesh != nullptr);

		if (hasMesh && ImGui::Button(rc->mesh->path.c_str()))
				ImGuiFileDialog::Instance()->OpenModal("ChooseFileModel", "Choose File", ".obj\0.fbx\0.3ds\0.dae\0.FBX\0", ".");
		else if(!hasMesh && ImGui::Button("(Empty)"))
				ImGuiFileDialog::Instance()->OpenModal("ChooseFileModel", "Choose File", ".obj\0.fbx\0.3ds\0.dae\0.FBX\0", ".");
		
		if (hasMesh)	DisplayInfo(rc->mesh->path.c_str());

		ImGui::Checkbox("##cs ", &rc->CastShadows);
		if(!rc->lightmapPath.empty())
			PrepName((char*)rc->lightmapPath.c_str());

		EndPreps();
	});

	if (remove)
	{
		obj.RemoveComponent<RendererComponent>();
	}
	material_editor(obj.GetComponent<RendererComponent>());
}
// -----------------------------------------------------------------------
void OpenTextureSelect()
{
	ImGuiFileDialog::Instance()->OpenModal("ChooseTexModel", "Choose Texture", ".png\0.bmp\0.tga\0.PNG\0.BMP\0.TGA\0.jpg\0.JPG\0.psd\0.tif", ".");
	//cout << ImGuiFileDialog::Instance()->GetCurrentPath() << endl;
}
// -----------------------------------------------------------------------
void  EditorProperty::EditTextureProprty(Material* tex)
{
	static int tex_index = 0;
	// Albedo Texture
	// -------------------------------------
	ImGui::PushID("SET_ALBEDO_TEXTURE");
	bool hasTex = (tex->tex_albedo != nullptr);
	if ((hasTex && ImGui::ImageButton((ImTextureID)tex->tex_albedo->getTexID(), ImVec2(50.f, 50.f)))
		|| (!hasTex && ImGui::Button("(None)", ImVec2(50.f, 50.f))))
	{
		OpenTextureSelect();
		tex_index = 1;
	}
	ImGui::PopID();
	if(hasTex)	DisplayInfo(tex->tex_albedo->getTexName().c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RES_FILE_TEX"))
		{
			IM_ASSERT(payload->DataSize == sizeof(std::string));
			std::string payload_n = *(std::string*)payload->Data;
			int slashPos = 0;
			for (size_t j = payload_n.length() - 1; j > 0; j--)
			{
				if (payload_n[j] == '\\')
				{
					slashPos = j;
					break;
				}
			}
			tex->tex_albedo = res->CreateTexture(payload_n.substr(slashPos + 1, std::string::npos), payload_n.c_str());
		}
		ImGui::EndDragDropTarget();
	}
	if (hasTex) {
		ImGui::SameLine();
		if (ImGui::Button("X ##AlbedoT"))
			tex->tex_albedo = nullptr;
	}
	// Normal Texture
	// -------------------------------------
	ImGui::PushID("SET_NORM_TEXTURE");
	hasTex = (tex->tex_normal != nullptr);
	if ((hasTex && ImGui::ImageButton((ImTextureID)tex->tex_normal->getTexID(), ImVec2(50.f, 50.f)))
		|| (!hasTex && ImGui::Button("(None)", ImVec2(50.f, 50.f))))
	{
		OpenTextureSelect();
		tex_index = 2;
	}
	ImGui::PopID();
	if (hasTex)	DisplayInfo(tex->tex_normal->getTexName().c_str());

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RES_FILE_TEX"))
		{
			IM_ASSERT(payload->DataSize == sizeof(std::string));
			std::string payload_n = *(std::string*)payload->Data;
			int slashPos = 0;
			for (size_t j = payload_n.length() - 1; j > 0; j--)
			{
				if (payload_n[j] == '\\')
				{
					slashPos = j;
					break;
				}
			}
			tex->tex_normal = res->CreateTexture(payload_n.substr(slashPos + 1, std::string::npos), payload_n.c_str());
		}
		ImGui::EndDragDropTarget();
	}
	if (hasTex) {
		ImGui::SameLine();
		if (ImGui::Button("X ##NormT"))
			tex->tex_normal = nullptr;
	}

	// Metall Texture
	// -------------------------------------
	ImGui::PushID("SET_META_TEXTURE");
	hasTex = (tex->tex_metal != nullptr);
	if ((hasTex && ImGui::ImageButton((ImTextureID)tex->tex_metal->getTexID(), ImVec2(50.f, 50.f)))
		|| (!hasTex && ImGui::Button("(None)", ImVec2(50.f, 50.f))))
	{
		OpenTextureSelect();
		tex_index = 3;
	}
	ImGui::PopID();
	if (hasTex)	DisplayInfo(tex->tex_metal->getTexName().c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RES_FILE_TEX"))
		{
			IM_ASSERT(payload->DataSize == sizeof(std::string));
			std::string payload_n = *(std::string*)payload->Data;
			int slashPos = 0;
			for (size_t j = payload_n.length() - 1; j > 0; j--)
			{
				if (payload_n[j] == '\\')
				{
					slashPos = j;
					break;
				}
			}
			tex->tex_metal = res->CreateTexture(payload_n.substr(slashPos + 1, std::string::npos), payload_n.c_str());
		}
		ImGui::EndDragDropTarget();
	}
	if (hasTex) {
		ImGui::SameLine();
		if (ImGui::Button("X ##MetalT"))
			tex->tex_metal = nullptr;
	}

	// Rough Texture
	// -------------------------------------
	ImGui::PushID("SET_ROUGH_TEXTURE");
	hasTex = (tex->tex_rough != nullptr);
	if ((hasTex && ImGui::ImageButton((ImTextureID)tex->tex_rough->getTexID(), ImVec2(50.f, 50.f)))
		|| (!hasTex && ImGui::Button("(None)", ImVec2(50.f, 50.f))))
	{
		OpenTextureSelect();
		tex_index = 4;
	}
	ImGui::PopID();
	if (hasTex)	DisplayInfo(tex->tex_rough->getTexName().c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RES_FILE_TEX"))
		{
			IM_ASSERT(payload->DataSize == sizeof(std::string));
			std::string payload_n = *(std::string*)payload->Data;
			int slashPos = 0;
			for (size_t j = payload_n.length() - 1; j > 0; j--)
			{
				if (payload_n[j] == '\\')
				{
					slashPos = j;
					break;
				}
			}
			tex->tex_rough = res->CreateTexture(payload_n.substr(slashPos + 1, std::string::npos), payload_n.c_str());
		}
		ImGui::EndDragDropTarget();
	}
	if (hasTex) {
		ImGui::SameLine();
		if (ImGui::Button("X ##roughT"))
			tex->tex_rough = nullptr;
	}

	// Emm Texture
	// -------------------------------------
	ImGui::PushID("SET_EMM_TEXTURE");
	hasTex = (tex->tex_emission != nullptr);
	if ((hasTex && ImGui::ImageButton((ImTextureID)tex->tex_emission->getTexID(), ImVec2(50.f, 50.f)))
		|| (!hasTex && ImGui::Button("(None)", ImVec2(50.f, 50.f))))
	{
		OpenTextureSelect();
		tex_index = 5;
	}
	ImGui::PopID();
	if (hasTex)	DisplayInfo(tex->tex_emission->getTexName().c_str());
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RES_FILE_TEX"))
		{
			IM_ASSERT(payload->DataSize == sizeof(std::string));
			std::string payload_n = *(std::string*)payload->Data;
			int slashPos = 0;
			for (size_t j = payload_n.length() - 1; j > 0; j--)
			{
				if (payload_n[j] == '\\')
				{
					slashPos = j;
					break;
				}
			}
			tex->tex_emission = res->CreateTexture(payload_n.substr(slashPos + 1, std::string::npos), payload_n.c_str());
		}
		ImGui::EndDragDropTarget();
	}
	if (hasTex) {
		ImGui::SameLine();
		if (ImGui::Button("X ##emmT"))
			tex->tex_emission = nullptr;
	}

	if (ImGuiFileDialog::Instance()->FileDialog("ChooseTexModel"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk == true)
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			RGetRelativePath(filePathName);
			RGetRelativePath(filePath);

			std::string fileName = filePathName;
			eraseSubStr(fileName, filePath);
			eraseSubStr(fileName, string("\\"));

			if (tex_index == 1)
				tex->tex_albedo = res->CreateTexture(fileName, filePathName.c_str());
			if (tex_index == 2)
				tex->tex_normal = res->CreateTexture(fileName, filePathName.c_str());
			if (tex_index == 3)
				tex->tex_metal = res->CreateTexture(fileName, filePathName.c_str());
			if (tex_index == 4)
				tex->tex_rough = res->CreateTexture(fileName, filePathName.c_str());
			if (tex_index == 5)
				tex->tex_emission = res->CreateTexture(fileName, filePathName.c_str());
		}
		// close
		ImGuiFileDialog::Instance()->CloseDialog("ChooseTexModel");
	}
}
// -----------------------------------------------------------------------
void EditorProperty::dir_light_editor(Entity &obj)
{
	DrawComponent<DirectionalLight>("Directional Light", &obj,
		// Draw Properties
		[](Entity* ent, DirectionalLight* dir_light, bool removeComponent)
	{
		BeginPreps("edit_dl");
		PrepName("enabled");
		PrepName("Intensity");
		PrepName("Color");
		PrepName("Cast Shadows");
		PrepName("Soft Shadows");
		PrepName("Bais");
		NextPreps();
		ImGui::Checkbox("##DirLight__enable", &dir_light->enabled);
		SetWeightPrep(); ImGui::DragFloat("##DirLight_Intensity", &dir_light->Intensity, 0.01f);
		ImGui::ColorEdit3("##DirLight_Color", (float*)&dir_light->Color, ImGuiColorEditFlags_NoInputs);
		ImGui::Checkbox("##DirLight_active", &dir_light->CastShadows);
		ImGui::Checkbox("##DirLight_soft", &dir_light->Soft);
		SetWeightPrep(); ImGui::DragFloat("##DirLight_bais", &dir_light->Bias, 0.001f);
		EndPreps();

		if (removeComponent)
			ent->RemoveComponent<DirectionalLight>();
	});
}
// -----------------------------------------------------------------------
void EditorProperty::point_light_editor(Entity& obj)
{
	DrawComponent<PointLight>("Point Light", &obj, 
	// Draw Properties
		[](Entity* ent, PointLight* point_light, bool removeComponent)
	{
		BeginPreps("edit_pl");
		PrepName("Active");
		PrepName("Intensity");
		PrepName("Color");
		PrepName("Raduis");
		PrepName("Cast Shadows");
		PrepName("Bias");
		NextPreps();
		ImGui::Checkbox("##ppLight_enable", &point_light->enabled);
		ImGui::DragFloat("##PLight_Intensity", &point_light->Intensity, 0.01f);
		SetWeightPrep();
		ImGui::ColorEdit3("##PLight_Color", (float*)&point_light->Color, ImGuiColorEditFlags_NoInputs);

		SetWeightPrep(); ImGui::DragFloat("##PLight_Linear", &point_light->Raduis, 0.01f);
		ImGui::Checkbox("##ppLight_active", &point_light->CastShadows);
		SetWeightPrep(); ImGui::DragFloat("##PLight_bias", &point_light->Bias, 0.01f);
		EndPreps();
		//ImGui::Checkbox("inFrustum", &point_light->inFrustum);
		//bool yes = point_light->isActive();
		//ImGui::Checkbox("isActive()", &yes);
		if (removeComponent)
			ent->RemoveComponent<PointLight>();
	});
}
// -----------------------------------------------------------------------
void EditorProperty::spot_light_editor(Entity &obj)
{
	DrawComponent<SpotLight>("Spot Light", &obj,
		// Draw Properties
		[](Entity* ent, SpotLight* spot_light, bool removeComponent)
	{
		BeginPreps("edit_pl");
		PrepName("Enabled");
		PrepName("Intensity");
		PrepName("Color");
		PrepName("Raduis");
		PrepName("Cut Off");
		PrepName("Outer Cut Off");
		PrepName("Cast Shadows");
		PrepName("Bias");
		NextPreps();
		ImGui::Checkbox("##spLight_enable", &spot_light->enabled);
		ImGui::DragFloat("##PsLight_Intensity", &spot_light->Intensity, 0.01f);
		SetWeightPrep();
		ImGui::ColorEdit3("##PsLight_Color", (float*)&spot_light->Color, ImGuiColorEditFlags_NoInputs);

		SetWeightPrep(); ImGui::DragFloat("##PsLight_rLinear", &spot_light->Raduis, 0.01f);
		SetWeightPrep(); ImGui::DragFloat("##PsLight_rad", &spot_light->CutOff, 0.1f);
		SetWeightPrep(); ImGui::DragFloat("##PsLight_orad", &spot_light->OuterCutOff, 0.1f);

		ImGui::Checkbox("##sssppLight_active", &spot_light->CastShadows);
		SetWeightPrep(); ImGui::DragFloat("##ssPLight_bias", &spot_light->Bias, 0.01f);
		ImGui::DragInt("##PsLight_rLiaanear", (int*)&spot_light->shadow_index);
		//ImGui::Checkbox("VISIBLE", &spot_light->visible);
		EndPreps();

		if (removeComponent)
			ent->RemoveComponent<SpotLight>();
	});
}
// -----------------------------------------------------------------------
void EditorProperty::ref_probe_editor(Entity &obj)
{
	DrawComponent<ReflectionProbe>("Reflection Probe", &obj,
		// Draw Properties
		[](Entity* ent, ReflectionProbe* ref_probe, bool removeComponent)
	{
		BeginPreps("edit_ple");
		PrepName("Intensity");
		PrepName("Static Only");
		PrepName("Transform Scale");
		PrepName("Box Projection");
		PrepName("Box");
		PrepName("");
		PrepName("Resolution");
		NextPreps();
		ImGui::DragFloat("##rp_Intensity", &ref_probe->Intensity, 0.01f);
		ImGui::Checkbox("##prpLight_sos", &ref_probe->static_only);
		ImGui::Checkbox("##prpLight_ssos", &ref_probe->use_scale);
		ImGui::Checkbox("##prpLight_active", &ref_probe->BoxProjection);
		EditVec3_xyz("##rbox", ref_probe->box.BoxMin);
		EditVec3_xyz("##rbox2", ref_probe->box.BoxMax);
		SetWeightPrep(); ImGui::InputFloat("##rp_Resolution", &ref_probe->Resolution);
		EndPreps();
		if (ImGui::Button("Bake", ImVec2(ImGui::GetWindowWidth(), 0)))
			ref_probe->baked = false;

		if (removeComponent)
			ent->RemoveComponent<ReflectionProbe>();
	});
}
// -----------------------------------------------------------------------
void EditorProperty::rb_editor(Entity &obj)
{
	bool remove = DrawComponent<RigidBody>("Rigid Body", &obj, // Draw Properties
	[](Entity* ent, RigidBody* rigidbody, bool removeComponent)
	{
		BeginPreps("edit_rbody");
		PrepName("Mode");
		PrepName("Mass");
		PrepName("Liner Damping");
		PrepName("Anguler Damping");
		PrepName("Friction");
		PrepName("Bounciness");
		//PrepName("Gravity");
		PrepName("OnCollison Notification");
		NextPreps();

		int rbMode = 0;
		if (rigidbody->m_BodyMode == RB_STATIC)
			rbMode = 1;
		else if (rigidbody->m_BodyMode == RB_CHARACTAR)
			rbMode = 2;
		int seted_mode = rbMode;
		ImGui::Combo("##modeds_type", &seted_mode, "Rigid\0Static\0Charactar");
		if (rbMode != seted_mode) // Mode changed
			rigidbody->SetBodyMode((RPhyBodyMode)seted_mode);

		float mass = rigidbody->Mass;
		float ld = rigidbody->LinerDamping;
		float ad = rigidbody->AngulerDamping;
		SetWeightPrep(); ImGui::DragFloat("##rb_mass", &mass, 0.1f);
		SetWeightPrep(); ImGui::DragFloat("##rb_ld", &ld, 0.1f);
		SetWeightPrep(); ImGui::DragFloat("##rb_ad", &ad, 0.1f);
		rigidbody->SetMass(mass);
		rigidbody->SetDamping(ld, ad);

		ld = rigidbody->Friction;
		SetWeightPrep(); ImGui::DragFloat("##rb_Friction", &ld, 0.1f);
		rigidbody->SetFriction(ld);

		ld = rigidbody->Bounciness;
		SetWeightPrep(); ImGui::DragFloat("##rb_Bounciness", &ld, 0.1f);
		rigidbody->SetBounciness(ld);

		int cMode = 0;
		if (ent->m_CollMode == RB_STATIC)
			cMode = 1;
		else if (ent->m_CollMode == RB_CHARACTAR)
			cMode = 2;
		//float g = rigidbody->Gravity.y;
		//SetWeightPrep(); ImGui::DragFloat("##rb_gravity", &g, 0.1f);
		//if (g != rigidbody->Gravity.y)
			//rigidbody->SetGravityY(g);
		seted_mode = cMode;
		SetWeightPrep();	ImGui::Combo("##coll_modeds_type", &seted_mode, "None\0Once\0Always");
		if (cMode != seted_mode) // Mode changed
			ent->m_CollMode = ((RPhyBodyCollMode)seted_mode);
		EndPreps();

		ImGui::Button("Collision", ImVec2(ImGui::GetWindowWidth(), 0));
		BeginPreps("edit_rbodyColl");
		PrepName("Shape");
		PrepName("Offset");
		PrepName("Kinematic");
		PrepName("Trigger");
		if (rigidbody->m_CollisionType == SPHERE_COLLIDER)
			PrepName("Raduis");
		else if (rigidbody->m_CollisionType == CAPSULE_COLLIDER)
			PrepName("Raduis | Height");
		else if (rigidbody->m_CollisionType == BOX_COLLIDER)
			PrepName("Scale");

		else if (rigidbody->m_CollisionType == MESH_COLLIDER)
		{
			if (ImGui::Button("Set This Mesh"))
				rigidbody->smfs = true;
			PrepName("Scale");
		}
		NextPreps();
		int item = 0;
		if (rigidbody->m_CollisionType == SPHERE_COLLIDER)
			item = 1;
		else if (rigidbody->m_CollisionType == CAPSULE_COLLIDER)
			item = 2;
		else if (rigidbody->m_CollisionType == MESH_COLLIDER)
			item = 3;
		int seted = item;
		ImGui::Combo("##coll_type", &seted, "Box\0Sphere\0Capsule\0Mesh\0\0");
		if (item != seted) // Shape changed
		{
			rigidbody->ChangeShape((RCollisionShapeType)seted);
		}
		SetWeightPrep(); ImGui::DragFloat3("##rboffseteditrg", (float*)&rigidbody->mOffset, 0.01f);
		bool is_t = rigidbody->is_kinematic;
		ImGui::Checkbox("##rbTrkine", &is_t);
		if (is_t != rigidbody->is_kinematic)
		{
			rigidbody->SetKinematic(is_t);
		}
		is_t = rigidbody->is_trigger;
		ImGui::Checkbox("##rbTr", &is_t);
		if (is_t != rigidbody->is_trigger)
		{
			rigidbody->SetTrigger(is_t);
		}
		if (rigidbody->m_CollisionType == SPHERE_COLLIDER)
		{
			glm::vec3 rad = rigidbody->GetScale(); glm::vec3 rad_t = rad;
			SetWeightPrep(); ImGui::DragFloat("##rbc_rad", &rad.x, 0.1f);
			if (rad != rad_t) rigidbody->SetScale(rad);
		}
		else if (rigidbody->m_CollisionType == CAPSULE_COLLIDER)
		{
			glm::vec3 rad = rigidbody->GetScale(); glm::vec3 rad_t = rad;
			SetWeightPrep(); ImGui::DragFloat2("##rbc_rad", (float*)&rad, 0.1f);
			if (rad != rad_t) rigidbody->SetScale(rad);
		}
		else if (rigidbody->m_CollisionType == BOX_COLLIDER)
		{
			glm::vec3 rad = rigidbody->GetScale(); glm::vec3 rad_t = rad;
			SetWeightPrep(); ImGui::DragFloat3("##rbc_rad", (float*)&rad, 0.1f);
			if (rad != rad_t) rigidbody->SetScale(rad);
		}
		else if (rigidbody->m_CollisionType == MESH_COLLIDER)
		{
			if (rigidbody->collisionShape != nullptr)
				ImGui::Text(rigidbody->mesh_path.c_str());

			glm::vec3 rad = rigidbody->GetScale(); glm::vec3 rad_t = rad;
			SetWeightPrep(); ImGui::DragFloat3("##rbc_rad", (float*)&rad, 0.1f);
			if (rad != rad_t) rigidbody->SetScale(rad);
		}
		EndPreps();
	});
	if (remove)
		obj.RemoveComponent<RigidBody>();
	else if (obj.GetComponent<RigidBody>()->smfs)
	{
		obj.GetComponent<RigidBody>()->smfs = false;
		if (obj.HasComponent<RendererComponent>())
		{
			btTriangleMesh* m = phy_world->GetMeshCollider(obj.GetComponent<RendererComponent>()->mesh);
			obj.GetComponent<RigidBody>()->SetCollisionMesh(m, obj.GetComponent<RendererComponent>()->mesh->path, obj.GetComponent<RendererComponent>()->mesh->index);
		}
	}
}
// -----------------------------------------------------------------------
void EditorProperty::cam_editor(Entity &obj)
{
	bool remove = DrawComponent<CameraComponent>("Camera", &obj,
		// Draw Properties
		[](Entity* ent, CameraComponent* m_camera, bool removeComponent)
	{
		BeginPreps("edit_cam_comp");
		PrepName("Primary");
		PrepName("Field of view");
		PrepName("Far plane");
		PrepName("Near plane");
		NextPreps();
		ImGui::Checkbox("##camComp_primary", &m_camera->Primary);
		SetWeightPrep(); ImGui::DragFloat("##PCam_FOV", &m_camera->FOV, 0.01f);
		SetWeightPrep(); ImGui::DragFloat("##PCam_FARV", &m_camera->FarView, 0.01f);
		SetWeightPrep(); ImGui::DragFloat("##PCam_NEARV", &m_camera->NearView, 0.01f);

		EndPreps();
	});

	if (remove)
	{
		obj.RemoveComponent<CameraComponent>();
		rndr->m_cameras.RemoveComponent(obj.ID);
	}
}
// -----------------------------------------------------------------------
void EditorProperty::animation_editor(Entity & obj)
{
	bool remove = DrawComponent<SkeletalMeshComponent>("Skeletal Mesh", &obj,
		// Draw Properties
		[](Entity* ent, SkeletalMeshComponent* anim, bool removeComponent)
	{
		BeginPreps("edit_anim_comp");
		PrepName("Mesh");
		PrepName("Cast shadows");
		PrepName("Scale");
		PrepName("Speed");
		
		NextPreps();
		if (ImGui::Button("Set ##setRCmesh"))
			ImGuiFileDialog::Instance()->OpenModal("ChooseFileModelsk", "Choose File", ".fbx\0.obj\0.glb\0.dae\0\0", ".");

		if (anim->mesh) {
			ImGui::SameLine();
			ImGui::Text((char*)anim->mesh->mesh_dir.c_str());
		}
		ImGui::Checkbox("##skAnim_cs", &anim->CastShadows);
		SetWeightPrep(); ImGui::DragFloat("##sk_anim_scale", &anim->scale, 0.01f);
		SetWeightPrep(); ImGui::DragFloat("##sk_anim_speed", &anim->speed, 0.01f);
		EndPreps();
		if (ImGui::TreeNode("##meshesTree", "Meshs"))
		{
			for (size_t i = 0; i < anim->mesh->meshes.size(); i++)
			{
				ImGui::Separator();
				ImGui::Text(("Mesh " + to_string(i)).c_str());
				bool is_skiped = anim->isMeshSkiped(i);
				bool iss = is_skiped;
				ImGui::SameLine();
				ImGui::PushID(i);
				ImGui::Checkbox("Disable", &is_skiped);
				ImGui::PopID();
				if (iss != is_skiped && is_skiped == true)
					anim->SkipMesh(i);
				else if (iss != is_skiped && is_skiped == false)
					anim->UnSkipMesh(i);
			}
			ImGui::TreePop();
		}
		if (ImGui::Button("Add Animation", ImVec2(ImGui::GetWindowWidth(), 0.f)))
			ImGuiFileDialog::Instance()->OpenModal("ChooseFileModelsk_anim", "Choose File", ".obj\0.fbx\0.3ds\0.dae\0\0", ".");
		
		if (ImGui::TreeNode("##animTreee", "Animations"))
		{
			//std::string iid;
			for (size_t i = 0; i < anim->animations.size(); i++)
			{
				ImGui::Separator();
				ImGui::PushID(i);
				if (ImGui::Button("Play"))
					anim->PlayAnimation(anim->animations[i]->anim_name);
				ImGui::SameLine();
				ImGui::Text(anim->animations[i]->anim_path.c_str());
				/*ImGui::Separator();
				iid = "##";
				iid += anim->animations[i]->anim_name;
				ImGui::InputText(iid.c_str(), &anim->animations[i]->anim_name);
				iid = "Remove" + iid;
				if (ImGui::Button(iid.c_str()))
				{
					if (anim->animations.size() != 1)
						anim->RemoveAnimation(anim->animations[i]->anim_name);
				}
				ImGui::SameLine();
				ImGui::Text(anim->animations[i]->anim_path.c_str());*/
				ImGui::PopID();
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("##animMaterialss", "Materials"))
		{
			// Cheating
			
			if (!anim->material->isDefault)// || anim->materials.size() != 0 && anim->material->id != anim->materials[anim->materials.size() - 1]->id)
			{
				anim->materials.push_back(anim->material);
				anim->material->isDefault = true;
			}
			//std::cout << anim->material->id << " | " << anim->materials[anim->materials.size() - 1]->id << std::endl;
			for (size_t i = 0; i < anim->materials.size(); i++)
			{
				if (ImGui::Button(anim->materials[i]->getMatPath().c_str()))
				{ }
				/*if (ImGui::Button(std::string("X ##remove" + i).c_str()))
				{

				}*/
			}
			ImGui::TreePop();
		}
	});

	if (remove)
	{
		obj.RemoveComponent<SkeletalMeshComponent>();
		rndr->m_skMeshs.RemoveComponent(obj.ID);
	}
	material_editor(obj.GetComponent<SkeletalMeshComponent>());
}
// -----------------------------------------------------------------------
void EditorProperty::UIwidget_editor(Entity & obj)
{
	bool remove = DrawComponent<UIWidget>("UI Widget", &obj,
		// Draw Properties
		[](Entity* ent, UIWidget* uiw, bool removeComponent)
	{
		ImGui::Checkbox("Enabled ##ui_widgetEnabled", &uiw->enabled);
		ImGui::Button("UI General", ImVec2(ImGui::GetWindowWidth(), 0));
		BeginPreps("edit_ui_Widget");
		PrepName("Position");
		PrepName("Scale");
		PrepName("Rotation");
		PrepName("Color");
		PrepName("Border Rounding");
		PrepName("Alpha");
		PrepName("Blur");
		NextPreps();
		SetWeightPrep(); ImGui::DragFloat2("##UIWidget_pos", (float*)&uiw->Position, 1.0f);
		SetWeightPrep(); ImGui::DragFloat2("##UIWidget_scal", (float*)&uiw->Scale, 1.0f);
		SetWeightPrep(); ImGui::DragFloat("##UIWidget_rot", &uiw->Rotation, 0.1f);
		SetWeightPrep(); ImGui::ColorEdit4("##UIWidget_col", (float*)&uiw->color, ImGuiColorEditFlags_NoInputs);
		SetWeightPrep(); ImGui::DragFloat("##UIWidget_fround", &uiw->FrameRounding, 0.5f, 0.0f, 15.0f);
		SetWeightPrep(); ImGui::DragFloat("##UIWidget_tscalAlphaww", &uiw->Alpha, 0.1f, 0.0f, 1.0f);
		SetWeightPrep(); ImGui::Checkbox("##UIWidget_tblurp", &uiw->BlurBackground);
		EndPreps();
		if (ImGui::Button("UI Widgets", ImVec2(ImGui::GetWindowWidth(), 0)))
			ImGui::OpenPopup("ui_widget_modification");
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
		ImGui::BeginChild("UI Widgets", ImVec2(ImGui::GetWindowWidth(), 100));
		//std::string uniqName;
		for (size_t i = 0; i < uiw->widgets.size(); i++)
		{
			ImGui::PushID(i);
			bool selected = false;
			if (uiw->selectedWidget == uiw->widgets[i]->ID) selected = true;

			if (ImGui::Selectable(uiw->widgets[i]->name.c_str(), selected))
				uiw->selectedWidget = uiw->widgets[i]->ID;
			/*uniqName = uiw->widgets[i]->name;
			uniqName += "##";
			uniqName += uiw->widgets[i]->ID;
			if (ImGui::Selectable(uniqName.c_str(), selected))
				uiw->selectedWidget = uiw->widgets[i]->ID;*/
			ImGui::PopID();
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();
		if (ImGui::BeginPopup("ui_widget_modification"))
		{
			if (ImGui::Selectable("Text")) 
				uiw->AddUIWidget(UIWidgetType::UI_WIDGET_TEXT);
			if (ImGui::Selectable("Button"))
				uiw->AddUIWidget(UIWidgetType::UI_WIDGET_BUTTON);
			if (ImGui::Selectable("Image"))
				uiw->AddUIWidget(UIWidgetType::UI_WIDGET_IMAGE);
			if (ImGui::Selectable("Progress Bar"))
				uiw->AddUIWidget(UIWidgetType::UI_WIDGET_PROGRESSBAR);
			if (ImGui::Selectable("Line"))
				uiw->AddUIWidget(UIWidgetType::UI_WIDGET_LINE);
			if (ImGui::Selectable("Input Text"))
				uiw->AddUIWidget(UIWidgetType::UI_WIDGET_INPUT_TEXT);
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopup("ui_widget_remove"))
		{
			if (ImGui::Selectable("Remove widget"))
				uiw->RemoveUIWidget(uiw->selectedWidget);
			if (ImGui::Selectable("Duplicate widget"))
				uiw->DuplicateUIWidget(uiw->selectedWidget);
			ImGui::EndPopup();
		}
		if (ImGui::Button("Widget Editor", ImVec2(ImGui::GetWindowWidth(), 0)))
			ImGui::OpenPopup("ui_widget_remove");
		if (uiw->selectedWidget != 0)
		{
			for (size_t i = 0; i < uiw->widgets.size(); i++)
			{
				if (uiw->widgets[i]->ID == uiw->selectedWidget)
				{
					ImGui::Checkbox("Enabled##UIWidget_enable", &uiw->widgets[i]->Enabled);
					ImGui::Text("Alpha"); ImGui::SameLine(); ImGui::DragFloat("##UIWidget_atscal", &uiw->widgets[i]->Alpha, 0.1f, 0.0f, 1.0f);
					// TEXT EDIT
					// ------------------------------------------------------------------
					if (uiw->widgets[i]->type == UI_WIDGET_TEXT)
					{
						UIWidgetText* text = static_cast<UIWidgetText*>(uiw->widgets[i]);
						BeginPreps("edit_ui_text");
						PrepName("Name");
						PrepName("Position");
						PrepName("Size");
						PrepName("Scale");
						PrepName("Rotation");
						PrepName("Color");
						PrepName("Wrap");
						PrepName("Text");
						PrepName("Font Index");
						NextPreps();
						SetWeightPrep(); ImGui::InputText("##UIWidget_tname", &text->name);
						SetWeightPrep(); ImGui::DragFloat2("##UIWidget_tpos", (float*)&text->Position, 0.1f);
						SetWeightPrep(); ImGui::DragFloat2("##UIWidget_tsize", (float*)&text->Size, 0.1f);
						SetWeightPrep(); ImGui::DragFloat("##UIWidget_tscal", &text->Scale, 0.1f);
						SetWeightPrep(); ImGui::DragFloat("##UIWidget_trot", &text->Rotation, 0.1f);
						SetWeightPrep(); ImGui::ColorEdit4("##UIWidget_tcol", (float*)&text->color, ImGuiColorEditFlags_NoInputs);
						SetWeightPrep(); ImGui::Checkbox("##UIWidget_twrap", &text->Wrap);
						SetWeightPrep(); ImGui::InputTextMultiline("##UIWidget_ttext", &text->text);
						SetWeightPrep(); ImGui::DragInt("##UIWidget_tfont", (int*)&text->fontIndex, 1);
						EndPreps();
						break;
					}
					// Button EDIT
					// ------------------------------------------------------------------
					else if (uiw->widgets[i]->type == UI_WIDGET_BUTTON)
					{
						UIWidgetButton* button = static_cast<UIWidgetButton*>(uiw->widgets[i]);
						BeginPreps("edit_ui_button");
						PrepName("Name");
						PrepName("Text");
						PrepName("Position");
						PrepName("Size");
						PrepName("Scale");
						PrepName("Text Color");
						PrepName("Color");
						PrepName("Hovered Color");
						PrepName("Press Color");
						PrepName("Frame Rounding");
						PrepName("Image");
						PrepName("OnClick Invoke");
						PrepName("Font Index");
						//PrepName("Text as Button");
						NextPreps();
						SetWeightPrep(); ImGui::InputText("##UIWidget_tname", &button->name);
						SetWeightPrep(); ImGui::InputText("##UIWidget_ttext", &button->text);
						SetWeightPrep(); ImGui::DragFloat2("##UIWidget_tpos", (float*)&button->Position, 0.1f);
						SetWeightPrep(); ImGui::DragFloat2("##UIWidget_tsize", (float*)&button->Size, 0.1f);
						SetWeightPrep(); ImGui::DragFloat("##UIWidget_tscal", &button->Scale, 0.1f);
						SetWeightPrep(); ImGui::ColorEdit4("##UIWidget_ttcol", (float*)&button->TextColor, ImGuiColorEditFlags_NoInputs);
						SetWeightPrep(); ImGui::ColorEdit4("##UIWidget_tcol", (float*)&button->Color, ImGuiColorEditFlags_NoInputs);
						SetWeightPrep(); ImGui::ColorEdit4("##UIWidget_tcolh", (float*)&button->HoveredColor, ImGuiColorEditFlags_NoInputs);
						SetWeightPrep(); ImGui::ColorEdit4("##UIWidget_tcolp", (float*)&button->PressColor, ImGuiColorEditFlags_NoInputs);
						SetWeightPrep(); ImGui::DragFloat("##UIWidget_tfrcolp", &button->FrameRounding, 0.1f, 0.0f, 100.0f);
						if (button->image != nullptr)
						{
							if (ImGui::Button("X##clearUIimage"))  button->image = nullptr;
							ImGui::SameLine();
							SetWeightPrep(); if (button->image != nullptr && ImGui::Button(button->image->getTexName().c_str())) { ImGuiFileDialog::Instance()->OpenModal("SetUIImagetex", "Choose Image", ".png\0.jpg\0.bmp\0.tga", "."); }
						}
						else {
							SetWeightPrep(); if (ImGui::Button("(None)")) { ImGuiFileDialog::Instance()->OpenModal("SetUIImagetex", "Choose Image", ".png\0.jpg\0.bmp\0.tga", "."); }
						}
						SetWeightPrep(); ImGui::InputText("##UIWidget_onclickF", &button->invokeFunc);
						SetWeightPrep(); ImGui::DragInt("##UIWidget_tfont", (int*)&button->fontIndex, 1);
						//SetWeightPrep(); ImGui::Checkbox("##UIWidget_txtasb", &button->TextAsButton);
						EndPreps();
						break;
					}
					// Image EDIT
					// ------------------------------------------------------------------
					else if (uiw->widgets[i]->type == UI_WIDGET_IMAGE)
					{
						UIWidgetImage* image = static_cast<UIWidgetImage*>(uiw->widgets[i]);
						BeginPreps("edit_ui_image");
						PrepName("Name");
						PrepName("Position");
						PrepName("Size");
						PrepName("Color");
						PrepName("Image");
						PrepName("FrameRounding");
						NextPreps();
						SetWeightPrep(); ImGui::InputText("##UIWidget_tname", &image->name);
						SetWeightPrep(); ImGui::DragFloat2("##UIWidget_tpos", (float*)&image->Position, 0.1f);
						SetWeightPrep(); ImGui::DragFloat2("##UIWidget_tsize", (float*)&image->Size, 0.1f);
						SetWeightPrep(); ImGui::ColorEdit4("##UIWidget_tcol", (float*)&image->Color, ImGuiColorEditFlags_NoInputs);
						if (image->image != nullptr)
						{
							if (ImGui::Button("X##clearUIimage"))  image->image = nullptr;
							ImGui::SameLine();
							SetWeightPrep(); if (image->image != nullptr && ImGui::Button(image->image->getTexName().c_str())) { ImGuiFileDialog::Instance()->OpenModal("SetUIImagetex", "Choose Image", ".png\0.jpg\0.bmp\0.tga", "."); }
						}
						else {
							SetWeightPrep(); if (ImGui::Button("(None)")) { ImGuiFileDialog::Instance()->OpenModal("SetUIImagetex", "Choose Image", ".png\0.jpg\0.bmp\0.tga", "."); }
						}

						SetWeightPrep(); ImGui::DragFloat("##UIWidget_tframround", &image->FrameRounding, 0.1f, 0.0f, 100.0f);
						EndPreps();
						break;
					}
					// Progress Bar EDIT
					// ------------------------------------------------------------------
					else if (uiw->widgets[i]->type == UI_WIDGET_PROGRESSBAR)
					{
						UIWidgetProgressBar* pbar = static_cast<UIWidgetProgressBar*>(uiw->widgets[i]);
						BeginPreps("edit_ui_pbar");
						PrepName("Name");
						PrepName("Position");
						PrepName("Size");
						PrepName("Value");
						PrepName("Bar Color");
						PrepName("Color");
						PrepName("Right To Left");
						PrepName("FrameRounding");
						NextPreps();
						SetWeightPrep(); ImGui::InputText("##UIWidget_tname", &pbar->name);
						SetWeightPrep(); ImGui::DragFloat2("##UIWidget_tpos", (float*)&pbar->Position, 0.1f);
						SetWeightPrep(); ImGui::DragFloat2("##UIWidget_tsize", (float*)&pbar->Size, 0.1f);
						SetWeightPrep(); ImGui::DragFloat("##UIWidget_tva", &pbar->Value, 0.01f, 0.0f, 1.0f);
						SetWeightPrep(); ImGui::ColorEdit4("##UIWidget_tbarcol", (float*)&pbar->BarColor, ImGuiColorEditFlags_NoInputs);
						SetWeightPrep(); ImGui::ColorEdit4("##UIWidget_tcol", (float*)&pbar->Color, ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox("##UIWidget_trtl", &pbar->RightToLeft);
						SetWeightPrep(); ImGui::DragFloat("##UIWidget_tframround", &pbar->FrameRounding, 0.1f, 0.0f, 100.0f);
						EndPreps();
						break;
					}
					// Line EDIT
					// ------------------------------------------------------------------
					else if (uiw->widgets[i]->type == UI_WIDGET_LINE)
					{
						UIWidgetLine* pline = static_cast<UIWidgetLine*>(uiw->widgets[i]);
						BeginPreps("edit_ui_pline");
						PrepName("Name");
						PrepName("Position");
						PrepName("Size");
						PrepName("Thickness");
						PrepName("Color");
						PrepName("Vertical line");
						PrepName("Custom");
						NextPreps();
						SetWeightPrep(); ImGui::InputText("##UIWidget_linename", &pline->name);
						SetWeightPrep(); ImGui::DragFloat2("##UIWidget_linepos", (float*)&pline->Position, 0.1f);
						SetWeightPrep(); ImGui::DragFloat2("##UIWidget_linesize", (float*)&pline->Size, 0.1f);
						SetWeightPrep(); ImGui::DragFloat("##UIWidget_lineposl", &pline->thickness, 0.1f);
						SetWeightPrep(); ImGui::ColorEdit4("##UIWidget_linecol", (float*)&pline->Color, ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox("##UIWidget_tvrtl", &pline->VerticalLine);
						ImGui::Checkbox("##UIWidget_trtl", &pline->Custom);
						if (pline->Custom) {
							SetWeightPrep(); ImGui::DragFloat2("##UIWidget_lineP2", (float*)&pline->Point2, 0.1f);
						}
						EndPreps();
						break;
					}
					// InputText EDIT
					// ------------------------------------------------------------------
					else if (uiw->widgets[i]->type == UI_WIDGET_INPUT_TEXT)
					{
						UIWidgetInputText* itext = static_cast<UIWidgetInputText*>(uiw->widgets[i]);
						BeginPreps("edit_ui_itext");
						PrepName("Name");
						PrepName("Position");
						PrepName("Size");
						PrepName("Scale");
						PrepName("Color");
						PrepName("BG Color");
						PrepName("Hint Color");
						PrepName("Multiline");
						PrepName("Password");
						PrepName("Read only"); 
						PrepName("Hint");
						PrepName("text");
						PrepName("Font Index");
						NextPreps();
						SetWeightPrep(); ImGui::InputText("##UIWidget_text_ename", &itext->name);
						SetWeightPrep(); ImGui::DragFloat2("##UIWidget_text_pos", (float*)&itext->Position, 0.1f);
						SetWeightPrep(); ImGui::DragFloat2("##UIWidget_text_size", (float*)&itext->Size, 0.1f);

						SetWeightPrep(); ImGui::DragFloat("##UIWidget_text_posl", &itext->Scale, 0.1f);
						SetWeightPrep(); ImGui::ColorEdit4("##UIWidget_text_col", (float*)&itext->color, ImGuiColorEditFlags_NoInputs);
						SetWeightPrep(); ImGui::ColorEdit4("##UIWidget_text_bgcol", (float*)&itext->bgColor, ImGuiColorEditFlags_NoInputs);
						SetWeightPrep(); ImGui::ColorEdit4("##UIWidget_text_hintcol", (float*)&itext->HintColor, ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox("##UIWidget_itext_mult", &itext->Multiline);
						ImGui::Checkbox("##UIWidget_itext_pass", &itext->password);
						ImGui::Checkbox("##UIWidget_itext_ro", &itext->readOnly);
						SetWeightPrep(); ImGui::InputText("##UIWidget_Hintttext", &itext->Hint);
						SetWeightPrep(); ImGui::InputTextMultiline("##UIWidget_ttext", &itext->text);
						SetWeightPrep(); ImGui::DragInt("##UIWidget_tfont", (int*)&itext->fontIndex, 1);
						EndPreps();
						break;
					}
				}
			}
		}
	});

	if (remove)
	{
		obj.RemoveComponent<UIWidget>();
		rndr->m_cameras.RemoveComponent(obj.ID);
	}

	if (ImGuiFileDialog::Instance()->FileDialog("SetUIImagetex"))
	{
		if (ImGuiFileDialog::Instance()->IsOk == true)
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
			RGetRelativePath(filePathName);
			UIWidget* wdg = obj.GetComponent<UIWidget>();
			UIWidgetComponent* uic = wdg->GetUIWidgetRow(wdg->selectedWidget);
			if (uic != nullptr)
			{
				if (uic->type == UI_WIDGET_IMAGE)
					static_cast<UIWidgetImage*>(uic)->image = res->CreateTexture(filePathName, filePathName.c_str(), false);
				else if (uic->type == UI_WIDGET_BUTTON)
					static_cast<UIWidgetButton*>(uic)->image = res->CreateTexture(filePathName, filePathName.c_str(), false);
			}
		}
		ImGuiFileDialog::Instance()->CloseDialog("SetUIImagetex");
	}

}
// -----------------------------------------------------------------------
void EditorProperty::particles_editor(Entity &obj)
{
	bool remove = DrawComponent<ParticleSystem>("Particle System", &obj,
		// Draw Properties
		[](Entity* ent, ParticleSystem* comp, bool removeComponent)
	{
		BeginPreps("edit_particlesSystem");
		PrepName("Emitting");
		PrepName("Play On Start");
		PrepName("Looping");
		PrepName("Start LifeTime");
		PrepName("Start Speed");
		PrepName("Start Size");
		PrepName("Start Color");
		PrepName("Simulation Speed");
		PrepName("Gravity Modifier");
		PrepName("Max Particles");
		PrepName("One Direction");
		PrepName("Spread");
		PrepName("Emitte Count");
		PrepName("Add Size Over Time");
		NextPreps();
		ImGui::Checkbox("##psCompEmitting", &comp->Emitting);
		ImGui::Checkbox("##psCompPlayOnStart", &comp->PlayOnStart);
		ImGui::Checkbox("##psCompLooping", &comp->Looping);
		SetWeightPrep(); ImGui::DragFloat("##psCompStratLifeTime", &comp->StartLifeTime, 0.1f);
		SetWeightPrep(); ImGui::DragFloat2("##psCompStartSpeed", (float*)&comp->StartSpeed, 0.1f);
		SetWeightPrep(); ImGui::DragFloat2("##psCompStartSize", (float*)&comp->StartSize, 0.1f);
		SetWeightPrep(); ImGui::ColorEdit4("##psCompStartColor", (float*)&comp->StartColor);
		SetWeightPrep(); ImGui::DragFloat("##psCompSimulationSpeed", &comp->SimulationSpeed, 0.1f);
		SetWeightPrep(); ImGui::DragFloat3("##psCompGravityModifier", (float*)&comp->GravityModifier, 0.1f);
		int mp = comp->GetMaxParticleCount();
		SetWeightPrep(); ImGui::DragInt("##psCompMaxParticales", &mp);
		comp->SetMaxParticleCount(mp);
		ImGui::Checkbox("##psCompOneDirection", &comp->OneDirection);
		SetWeightPrep(); ImGui::DragFloat("##psCompSpread", &comp->Spread);
		SetWeightPrep(); ImGui::DragFloat("##psCompEmitteCount", &comp->EmitteCount, 1.0f);
		SetWeightPrep(); ImGui::DragFloat("##psCompAddSizeOverTime", &comp->AddSizeOverTime, 0.1f);
		EndPreps();

		if (ImGui::TreeNode("##ParticalesRenderingSector", "Rendering"))
		{
			ImGui::TreePop();
			BeginPreps("edit_particlesSystem_render");
			PrepName("Texture");
			PrepName("Use texture's alpha");
			PrepName("Fade out");
			PrepName("ReceiveShadows");
			NextPreps();
			if (ImGui::Button("Set ##pcsTex"))
				ImGuiFileDialog::Instance()->OpenModal("LoadTexForParticls", "Choose Texture", ".png\0.bmp\0.tga\0.PNG\0.BMP\0.TGA\0.jpg\0.JPG", ".");
			if (comp->mTexture)
			{
				ImGui::SameLine(); 
				ImGui::Text(comp->mTexture->getTexName().c_str());
			}
			ImGui::Checkbox("##psCompUseTexTransparent", &comp->UseTexTransparent);
			SetWeightPrep(); ImGui::DragFloat("##psCompFadeOut", &comp->FadeOut, 0.1f);
			ImGui::Checkbox("##psCompReceiveShadows", &comp->ReceiveShadows);
			EndPreps();
		}

		if (ImGui::TreeNode("##animsheetTexParticales", "Animation Sheet Texture"))
		{
			ImGui::TreePop();
			BeginPreps("edit_particlesSystem_anim");
			PrepName("Enabled");
			PrepName("Anim Sheet Size");
			PrepName("Anim Speed");
			NextPreps();
			ImGui::Checkbox("##psCompEanim_use", &comp->anim_use);
			SetWeightPrep(); ImGui::DragFloat2("##psCompAnimSheetSize", (float*)&comp->AnimSheetSize, 0.1f);
			SetWeightPrep(); ImGui::DragFloat("##psCompAnimSpeed", &comp->AnimSpeed, 0.1f);
			EndPreps();
		}
	});

	if (remove)
		obj.RemoveComponent<ParticleSystem>();

	if (ImGuiFileDialog::Instance()->FileDialog("LoadTexForParticls"))
	{
		if (ImGuiFileDialog::Instance()->IsOk == true)
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
			RGetRelativePath(filePathName);
			obj.GetComponent<ParticleSystem>()->mTexture = res->CreateTexture(filePathName, filePathName.c_str(), true);
		}
		ImGuiFileDialog::Instance()->CloseDialog("LoadTexForParticls");
	}
}
// -----------------------------------------------------------------------
void EditorProperty::audio_editor(Entity & obj)
{
	bool remove = DrawComponent<RAudioSource>("Audio Source", &obj,
		// Draw Properties
		[](Entity* ent, RAudioSource* comp, bool removeComponent)
	{
		BeginPreps("edit_audioSource");
		PrepName("Clip");
		PrepName("2D");
		PrepName("Looping");
		PrepName("Play on Start");
		PrepName("Volume");
		PrepName("Min Distance");
		PrepName("Max Distance");
		PrepName("Overlap");
		PrepName("Echo Filter");
		if (ImGui::Button("Play", ImVec2(ImGui::GetWindowWidth() / 2, 0)))
			comp->Play();
		NextPreps();
		if (ImGui::Button("Set ##audioS"))
			ImGuiFileDialog::Instance()->OpenModal("LoadanAudio", "Choose Audio Clip", ".wav\0.mp3\0.ogg", ".");
		if (comp->mClip != nullptr)
		{
			ImGui::SameLine();
			ImGui::Text(comp->mClip->mPath.c_str());
		}
		ImGui::Checkbox("##psCompRAlooping2d", &comp->Is2D);

		float vol = comp->GetVolume();
		float minD = comp->GetMinDistance();
		float maxD = comp->GetMaxDistance();
		float ef = comp->GetEchoFilter();
		bool looping = comp->GetLooping();
		ImGui::Checkbox("##psCompRAlooping", &looping);
		ImGui::Checkbox("##psCompRPlayOnStart", &comp->PlayOnStart);
		SetWeightPrep(); ImGui::DragFloat("##psCompAVol", &vol, 0.1f);
		SetWeightPrep(); ImGui::DragFloat("##psCompAMin", &minD, 0.1f);
		SetWeightPrep(); ImGui::DragFloat("##psCompAMax", &maxD, 0.1f);
		ImGui::Checkbox("##psCompAefovap", &comp->Overlap);
		SetWeightPrep(); ImGui::DragFloat("##psCompAef", &ef, 0.1f);
		comp->SetVolume(vol);
		comp->SetMinDistance(minD);
		comp->SetMaxDistance(maxD);
		comp->SetLooping(looping);
		comp->SetEchoFilter(ef);
		if (ImGui::Button("Stop", ImVec2(ImGui::GetWindowWidth() / 2, 0)))
			comp->Stop();
		EndPreps();
	});

	if (remove)
		obj.RemoveComponent<RAudioSource>();

	if (ImGuiFileDialog::Instance()->FileDialog("LoadanAudio"))
	{
		if (ImGuiFileDialog::Instance()->IsOk == true)
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
			RGetRelativePath(filePathName);
			obj.GetComponent<RAudioSource>()->mClip = audio_mnger->LoadClip(filePathName);
		}
		ImGuiFileDialog::Instance()->CloseDialog("LoadanAudio");
	}
}
// -----------------------------------------------------------------------
void EditorProperty::grass_editor(Entity & obj)
{
	bool remove = DrawComponent<GrassComponent>("Grass Renderer", &obj,
		// Draw Properties
		[](Entity* ent, GrassComponent* comp, bool removeComponent)
	{
		BeginPreps("edit_grassRC");
		PrepName("Texture");
		PrepName("Alpha");
		PrepName("Size");
		PrepName("Distance");
		NextPreps();
		if (ImGui::Button("Set ##grassTex"))
			ImGuiFileDialog::Instance()->OpenModal("SetGrassTex", "Choose Grass Texture", ".png\0.jpg\0.bmp\0.tga", ".");
		if (comp->mTexture != nullptr)
		{
			ImGui::SameLine();
			ImGui::Text(comp->mTexture->getTexName().c_str());
		}
		SetWeightPrep(); ImGui::DragFloat("##psCompTexGrassAlps", &comp->alpha, 0.01f);
		SetWeightPrep(); ImGui::DragFloat2("##psCompTexgrassSize", (float*)&comp->size, 0.01f);
		SetWeightPrep(); ImGui::DragFloat("##psCompTexGrassdis", &comp->Distance, 0.1f);
		EndPreps();
		ImGui::Button("Editing", ImVec2(ImGui::GetWindowWidth(), 0));
		ImGui::Checkbox("Edit ##editgrassineditor", &comp->edit);
		SetWeightPrep(); ImGui::DragInt("Amount ##psCompTexGrassEditt01", (int*)&comp->Edit_amount, 1, 1);
		SetWeightPrep(); ImGui::DragInt("Raduis ##psCompTexGrassEditt01", (int*)&comp->Edit_Raduis, 1, 1);
	});

	if (remove)
		obj.RemoveComponent<GrassComponent>();

	if (ImGuiFileDialog::Instance()->FileDialog("SetGrassTex"))
	{
		if (ImGuiFileDialog::Instance()->IsOk == true)
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
			RGetRelativePath(filePathName);
			obj.GetComponent<GrassComponent>()->mTexture = res->CreateTexture(filePathName, filePathName.c_str(), true);
		}
		ImGuiFileDialog::Instance()->CloseDialog("SetGrassTex");
	}
}
// -----------------------------------------------------------------------
SceneManager* ascn;

void editDisp(Entity* ent, Terrain* comp, bool removeComponent)
{
	if (ImGui::BeginTabBar("TerrainEditTabBar", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("Settings"))
		{
			EditorProperty::BeginPreps("edit_terrain_props");
			EditorProperty::PrepName("Size");
			EditorProperty::PrepName("MaxHeight");
			EditorProperty::PrepName("FrustumCulling");
			EditorProperty::PrepName("DEBUG");

			EditorProperty::NextPreps();
			EditorProperty::SetWeightPrep(); ImGui::DragFloat("##terrain_Size", &comp->Size);
			EditorProperty::SetWeightPrep(); ImGui::DragFloat("##terrain_MaxHeight", &comp->MaxHeight);
			ImGui::Checkbox("##frcc", &comp->FrustumCulling);
			EditorProperty::SetWeightPrep(); ImGui::DragInt("##terrain_dbg", &comp->Debug);
		

			EditorProperty::EndPreps();

			if (ImGui::Button("Update ##editDisp", ImVec2(ImGui::GetWindowWidth(), 0)))
				comp->UpdateMesh();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("LOD levels"))
		{
			EditorProperty::BeginPreps("edit_terrain_lods");
			EditorProperty::PrepName("Decimate");
			EditorProperty::PrepName("LOD0 Distance");
			EditorProperty::PrepName("LOD1 Distance");
			EditorProperty::PrepName("LOD2 Distance");
			EditorProperty::PrepName("LOD3 Distance");

			EditorProperty::NextPreps();
			EditorProperty::SetWeightPrep(); ImGui::DragFloat("##terrain_Decimate", &comp->Decimate);
			EditorProperty::SetWeightPrep(); ImGui::DragFloat("##LOD0_distance", &comp->LOD0_distance);
			EditorProperty::SetWeightPrep(); ImGui::DragFloat("##LOD1_distance", &comp->LOD1_distance);
			EditorProperty::SetWeightPrep(); ImGui::DragFloat("##LOD2_distance", &comp->LOD2_distance);
			EditorProperty::SetWeightPrep(); ImGui::DragFloat("##LOD3_distance", &comp->LOD3_distance);

			EditorProperty::EndPreps();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Painting"))
		{
			EditorProperty::BeginPreps("edit_terrain_paints");
			EditorProperty::PrepName("Splatmap");
			EditorProperty::PrepName("Materials ");
			ImGui::SameLine(); 
			ImGui::Text(std::to_string(0).c_str());
			ImGui::SameLine();
			ImGui::Text("/");
			ImGui::SameLine();
			ImGui::Text(std::to_string(4).c_str());
			EditorProperty::NextPreps();
			EditorProperty::SetWeightPrep(); ImGui::Button("(None)");
			EditorProperty::SetWeightPrep(); ImGui::Button("  Add  ");
			ImGui::SameLine(); ImGui::Button("Remove");
			EditorProperty::EndPreps();

			//if (ImGui::Button("Materials", ImVec2(ImGui::GetWindowWidth(), 0)))
				//ImGui::OpenPopup("terrain_add_mat");
			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
			ImGui::BeginChild("terrain materials", ImVec2(ImGui::GetWindowWidth(), 100));
			 
			ImGui::EndChild();
			ImGui::PopStyleColor();

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Trees"))
		{
			EditorProperty::BeginPreps("edit_terrain_trees");
			EditorProperty::PrepName("Prefabs ");
			EditorProperty::NextPreps();
			EditorProperty::SetWeightPrep(); ImGui::Button("  Add  ");
			ImGui::SameLine(); ImGui::Button("Remove");
			EditorProperty::EndPreps();

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
			ImGui::BeginChild("Terrain prefabs", ImVec2(ImGui::GetWindowWidth(), 100));

			ImGui::EndChild();
			ImGui::PopStyleColor();

			ImGui::EndTabItem();
		}
		/*if (ImGui::BeginTabItem("Edit"))
		{
			if (ImGui::Button("Edit ##editDisp", ImVec2(ImGui::GetWindowWidth(), 0)))
			{
				ascn->editDisp = ent->ID;
				ascn->PushRequest(SR_EDIT_DISP);
			}
			EditorProperty::BeginPreps("edit_Displacement");
			EditorProperty::PrepName("Raduis");
			EditorProperty::PrepName("Intensity");
			EditorProperty::PrepName("DoOneHeight");
			EditorProperty::PrepName("Height");
			EditorProperty::PrepName("Set Alpha");
			EditorProperty::NextPreps();
			EditorProperty::SetWeightPrep(); ImGui::DragFloat("##disp_ee", &comp->Raduis, 0.1f, 0.0f);
			EditorProperty::SetWeightPrep(); ImGui::DragFloat("##disp_eee", &comp->Intensity, 0.1f, 0.0f, 100.0f);
			ImGui::Checkbox("##disp_eeee", &comp->useOneHeight);
			EditorProperty::SetWeightPrep(); ImGui::DragFloat("##disp_eeeee", &comp->SameHeight, 0.1f, 0.0f, 100.0f);
			ImGui::Checkbox("##disp_eeeeee", &comp->edit_alpha);
			EditorProperty::EndPreps();

			ImGui::EndTabItem();
		}*/
		ImGui::EndTabBar();
	}

	ImGui::Separator();
	//EditorProperty::PrepName("VertexCount: ");
	//ImGui::SameLine();
	//ImGui::TextColored(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), std::to_string(comp->VertexCount).c_str());
}

void EditorProperty::cpp_script_editor(Entity & obj)
{
	bool remove = DrawComponent<NativeScriptComponent>("CppTest (C++)", &obj,
	// Draw Properties
	[](Entity* ent, NativeScriptComponent* comp, bool removeComponent)
	{
		BeginPreps("edit_cppscr");
		//comp->Instance->varNames[0] = "how";
		for (size_t i = 0; i < comp->Instance->varsNames.size(); i++)
		{
			PrepName(comp->Instance->varsNames[i].c_str());
			std::cout << "varNames " << i << " " << comp->Instance->varsNames[i] << "\n";
		}
		//PrepName("speed");
		NextPreps();
		for (size_t i = 0; i < comp->Instance->varsNames.size(); i++)
		{
			auto vname = comp->Instance->varsNames[i];
			SetWeightPrep();
			auto var_type = (ScriptableEntity::NScriptVarType)comp->Instance->varsType[i];
			if (var_type == ScriptableEntity::NScriptVarType::NSVT_FLOAT) {
				ImGui::DragFloat(("##cpp_speed" + vname).c_str(), (float*)comp->Instance->vars[i], 0.01f);
			}
			else if (var_type == ScriptableEntity::NScriptVarType::NSVT_INT){
				ImGui::DragInt(("##cpp_speed" + vname).c_str(), (int*)comp->Instance->vars[i], 0.01f);
			}
			else if (var_type == ScriptableEntity::NScriptVarType::NSVT_BOOL) {
				ImGui::Checkbox(("##cpp_speed" + vname).c_str(), (bool*)comp->Instance->vars[i]);
			}
			else if (var_type == ScriptableEntity::NScriptVarType::NSVT_STR) {
				ImGui::InputText(("##cpp_speed" + vname).c_str(), (string*)comp->Instance->vars[i]);
			}
			else
				std::cout << "avar is " << vname << " " << comp->Instance->varsType[i] << "\n";
		}
		//SetWeightPrep(); ImGui::DragFloat("##cpp_speed", &((CppTest*)comp->Instance)->speed, 0.01f);
		EndPreps();
	});

	if (remove)
		obj.RemoveComponent<NativeScriptComponent>();
}
// -----------------------------------------------------------------------
void EditorProperty::terrainCollider_editor(Entity & obj)
{
	bool remove = DrawComponent<TerrainCollider>("Terrain Collider", &obj,
		// Draw Properties
		[](Entity* ent, TerrainCollider* comp, bool removeComponent)
	{
		BeginPreps("edit_terrainCol");
		NextPreps();
		EndPreps();
	});

	if (remove)
		obj.RemoveComponent<TerrainCollider>();
}
// -----------------------------------------------------------------------
void EditorProperty::displacement_editor(Entity & obj)
{
	ascn = nodes->scene;
	bool remove = DrawComponent<Terrain>("Terrain", &obj, editDisp);

	if (remove)
		obj.RemoveComponent<Terrain>();
}
// -----------------------------------------------------------------------
void EditorProperty::components_editor(Entity &obj)
{
	for (size_t i = 0; i < obj.m_components.size(); i++)
	{
		if (Component::IsComponentType<RendererComponent>(obj.m_components[i]))
			mesh_editor(obj);
		else if (Component::IsComponentType<BillboardComponent>(obj.m_components[i]))
			billboard_editor(obj);
		else if (Component::IsComponentType<PointLight>(obj.m_components[i]))
			point_light_editor(obj);
		else if (Component::IsComponentType<DirectionalLight>(obj.m_components[i]))
			dir_light_editor(obj);
		else if (Component::IsComponentType<SpotLight>(obj.m_components[i]))
			spot_light_editor(obj);
		else if (Component::IsComponentType<CameraComponent>(obj.m_components[i]))
			cam_editor(obj);
		else if (Component::IsComponentType<ReflectionProbe>(obj.m_components[i]))
			ref_probe_editor(obj);
		else if (Component::IsComponentType<RigidBody>(obj.m_components[i]))
			rb_editor(obj);
		else if (Component::IsComponentType<ParticleSystem>(obj.m_components[i]))
			particles_editor(obj);
		else if (Component::IsComponentType<RAudioSource>(obj.m_components[i]))
			audio_editor(obj);
		else if (Component::IsComponentType<GrassComponent>(obj.m_components[i]))
			grass_editor(obj);
		else if (Component::IsComponentType<UIWidget>(obj.m_components[i]))
			UIwidget_editor(obj);
		else if (Component::IsComponentType<SkeletalMeshComponent>(obj.m_components[i]))
			animation_editor(obj);
		else if (Component::IsComponentType<Terrain>(obj.m_components[i]))
			displacement_editor(obj);
		else if (Component::IsComponentType<NativeScriptComponent>(obj.m_components[i]))
			cpp_script_editor(obj);
		else if (Component::IsComponentType<TerrainCollider>(obj.m_components[i]))
			terrainCollider_editor(obj);
	}

	for (size_t i = 0; i < obj.m_scripts.size(); i++)
		scriptC_editor(obj, obj.m_scripts[i]);
}
// -----------------------------------------------------------------------

void EditorProperty::Render()
{
	ImGui::Begin("Properties", NULL, ImGuiWindowFlags_NoCollapse);
	//int a = rndr->m_SpotShadowMapper.shadowMaps.size();
	//int b = rndr->m_PointShadowMapper.shadowMaps.size();
	if (nodes->GetSelected() != -1)
	{
		EnttID oid = nodes->GetSelected();
		Entity* ourEntity = nodes->scene->FindEntity(oid);
		if (ourEntity == nullptr)
			nodes->sel_entt.clear();
		else {
			if (ourEntity->ID == oid)
			{
				ImGui::PushFont(nodes->icon_small);
				if (ImGui::Button("4")) // Save Prefab
				{
					if (ourEntity->IsPrefab())
						ourEntity->SaveEntityFile(ourEntity->path.c_str());
					else {
						save_ent_id = ourEntity->ID;
						ImGuiFileDialog::Instance()->OpenModal("SaveBPModel", "Save Prefab", ".rbp\0", ".");
					}
				}
				if (ImGui::IsItemClicked(1))
				{
					save_ent_id = ourEntity->ID;
					ImGui::OpenPopup("save_ent_pop");
				}

				ImGui::PopFont();
				ImGui::SameLine();
				ImGui::Text(std::to_string(ourEntity->ID).c_str());
				ImGui::SameLine();
				ImGui::Text("| ");
				ImGui::SameLine(); ImGui::Text(ourEntity->name.c_str());
				if (ourEntity->IsPrefab())
					ImGui::Text(ourEntity->path.c_str());
			
				general_editor(*ourEntity);
				transform_editor(*ourEntity);
				components_editor(*ourEntity);
			}
		}
	}
	else
	{
		ImGui::Text("");
		ImGui::Text("");
		ImGui::SameLine(0, ImGui::GetWindowWidth() / 2 - 200);
		ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 0.6f), "Select an object to see its properties.");
	}

	if (ImGui::BeginPopup("save_ent_pop"))
	{
		if (ImGui::Selectable("Save As"))
		{
			ImGuiFileDialog::Instance()->OpenModal("SaveBPModel", "Save Prefab", ".rbp\0", ".");
		}
		ImGui::EndPopup();
	}

	ImGui::End();

	if (ImGuiFileDialog::Instance()->FileDialog("SaveBPModel"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk == true)
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			RGetRelativePath(filePathName);

			// action
			if (save_ent_id != -1)
			{
				nodes->scene->SaveEntityPrefab(save_ent_id, filePathName.c_str());
				save_ent_id = -1;
			}
		}
		// close
		ImGuiFileDialog::Instance()->CloseDialog("SaveBPModel");
	}
}
// -----------------------------------------------------------------------