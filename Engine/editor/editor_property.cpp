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

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0.45f, 0.5f, 1));
		if (ImGui::Button("Add Component", ImVec2(ImGui::GetWindowWidth(), 0)))
			ImGui::OpenPopup("add_com");
		ImGui::PopStyleColor();

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
			ImGui::Separator();
			// # Scenes  ---------------------------------------------------------------------------
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 0.7f), "# Scene");
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
			// # Scripts  ---------------------------------------------------------------------------
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 0.7f), "# Custom (Scripts)");
			for (size_t i = 0; i < scrMnger->m_loaded_scripts.size(); i++)
			{
				const char* className = scrMnger->m_loaded_scripts[i].class_name.c_str();
				if (scrMnger->m_loaded_scripts[i].valide && ImGui::Selectable(className))
				{
					obj.AddScript(scrMnger->InstanceComponentClass(obj.ID, className));
				}
			}
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

		if (ImGui::IsItemClicked(1))
			ImGui::OpenPopup("rmoveScr");

		if (is_on)
		{
			BeginPreps((script->class_name + "edit_scr").c_str());

			for (size_t i = 0; i < script->properties.size(); i++)
			{
				PrepName(script->properties[i].name.c_str());
			}

			NextPreps();
			for (size_t i = 0; i < script->properties.size(); i++)
			{
				const char* var_name = script->properties[i].name.c_str();
				std::string hide = "##";
				if (script->properties[i].type == VAR_Int)
				{
					int var = script->GetVarInt(var_name);
					int temp_var = var;
					SetWeightPrep();
					ImGui::DragInt(std::string(hide + script->class_name + var_name).c_str(), &var);
					if (var != temp_var)
						script->SetVar(var_name, var);
				}
				else if (script->properties[i].type == VAR_Float)
				{
					float var = script->GetVarFloat(var_name);
					float temp_var = var;
					SetWeightPrep();
					ImGui::DragFloat(std::string(hide + script->class_name + var_name).c_str(), &var, 0.1f);
					if (var != temp_var)
						script->SetVar(var_name, var);
				}
				else if (script->properties[i].type == VAR_String)
				{
					std::string var = script->GetVarString(var_name);
					std::string temp_var = var;
					SetWeightPrep();
					ImGui::InputText(std::string(hide + script->class_name + var_name).c_str(), &var);
					if (var != temp_var)
						script->SetVar(var_name, var);
				}
				else if (script->properties[i].type == VAR_Bool)
				{
					bool var = script->GetVarBool(var_name);
					bool temp_var = var;
					ImGui::Checkbox(std::string(hide + script->class_name + var_name).c_str(), &var);
					if (var != temp_var)
						script->SetVar(var_name, var);
				}
			}
			EndPreps();
			ImGui::Separator();
		}

		if (ImGui::BeginPopup("rmoveScr"))
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
void transform_editor(Entity &obj)
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		EditorProperty::BeginPreps("editTr");
		EditorProperty::PrepName("Position");
		EditorProperty::PrepName("Rotation");
		EditorProperty::PrepName("Scale");
		EditorProperty::NextPreps();

		glm::vec3 get_pos = obj.transform.GetLocalPosition();
		EditVec3_xyz("##pos", get_pos);
		obj.SetTranslation(get_pos, true);
		
		glm::vec3 get_ang = obj.transform.GetEurlerAngels();
		EditVec3_xyz("##rot", get_ang);
		//obj.transform.SetAngels(get_ang);
		obj.SetRotation(get_ang);

		glm::vec3 get_sca = obj.transform.GetLocalScale();
		EditVec3_xyz("##sca", get_sca);
		obj.SetScale(get_sca);

		EditorProperty::EndPreps();
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
		PrepName("Mesh");
		PrepName("Cast Shadows");

		NextPreps();

		if (ImGui::Button("Set ##setRCmesh"))
			ImGuiFileDialog::Instance()->OpenModal("ChooseFileModel", "Choose File", ".obj\0.fbx\0.3ds\0\0", ".");
		
		if (rc->mesh) {
			ImGui::SameLine();
			ImGui::Text((char*)rc->mesh->path.c_str());
		}

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
	ImGuiFileDialog::Instance()->OpenModal("ChooseTexModel", "Choose Texture", ".png\0.bmp\0.tga\0.PNG\0.BMP\0.TGA\0.jpg\0.JPG", ".");
	//cout << ImGuiFileDialog::Instance()->GetCurrentPath() << endl;
}
// -----------------------------------------------------------------------
void  EditorProperty::EditTextureProprty(Material* tex)
{
	static int tex_index = 0;
	// Albedo Texture
	// -------------------------------------
	if (ImGui::Button("Set ##alb"))
	{
		OpenTextureSelect();
		tex_index = 1;
	}
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
	ImGui::SameLine();

	if (tex->tex_albedo != nullptr)
	{
		if (ImGui::Button("Clear ##Albedo"))
		{
			res->RemoveTexture(tex->tex_albedo->getTexPath());
			tex->tex_albedo = nullptr;
		}
		else
		{
			ImGui::SameLine();
			ImGui::Text(tex->tex_albedo->getTexName().c_str());
		}
	}
	else ImGui::Text("(Empty)");

	// Normal Texture
	// -------------------------------------
	if (ImGui::Button("Set ##nor"))
	{
		OpenTextureSelect();
		tex_index = 2;
	}
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
	ImGui::SameLine();
	if (tex->tex_normal != nullptr)
	{
		if (ImGui::Button("Clear ##Normal"))
		{
			res->RemoveTexture(tex->tex_normal->getTexPath());
			tex->tex_normal = nullptr;
		}
		else
		{
			ImGui::SameLine();
			ImGui::Text(tex->tex_normal->getTexName().c_str());
		}
	}
	else ImGui::Text("(Empty)");

	// Metall Texture
	// -------------------------------------
	if (ImGui::Button("Set ##Metall"))
	{
		OpenTextureSelect();
		tex_index = 3;
	}
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
	ImGui::SameLine();
	if (tex->tex_metal != nullptr)
	{
		if (ImGui::Button("Clear ##tex_metal"))
		{
			res->RemoveTexture(tex->tex_metal->getTexPath());
			tex->tex_metal = nullptr;
		}
		else
		{
			ImGui::SameLine();
			ImGui::Text(tex->tex_metal->getTexName().c_str());
		}
	}
	else ImGui::Text("(Empty)");

	// Rough Texture
	// -------------------------------------
	if (ImGui::Button("Set ##Rough"))
	{
		OpenTextureSelect();
		tex_index = 4;
	}
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
	ImGui::SameLine();
	if (tex->tex_rough != nullptr)
	{
		if (ImGui::Button("Clear ##tex_rough"))
		{
			res->RemoveTexture(tex->tex_rough->getTexPath());
			tex->tex_rough = nullptr;
		}
		else
		{
			ImGui::SameLine();
			ImGui::Text(tex->tex_rough->getTexName().c_str());
		}
	}
	else ImGui::Text("(Empty)");

	// Emm Texture
	// -------------------------------------
	if (ImGui::Button("Set ##Emm"))
	{
		OpenTextureSelect();
		tex_index = 5;
	}
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
	ImGui::SameLine();
	if (tex->tex_emission != nullptr)
	{
		if (ImGui::Button("Clear ##tex_Emm"))
		{
			res->RemoveTexture(tex->tex_emission->getTexPath());
			tex->tex_emission = nullptr;
		}
		else
		{
			ImGui::SameLine();
			ImGui::Text(tex->tex_emission->getTexName().c_str());
		}
	}
	else ImGui::Text("(Empty)");

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
void EditorProperty::material_editor(RendererComponent* rc)
{
	if (!rc)
		return;

	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (rc->material->isDefault)
		{
			ImGui::Text("Path"); ImGui::SameLine(0, ImGui::GetWindowWidth()*0.2f);
			if (ImGui::Button("Default-Material"))
				ImGui::OpenPopup("mat_edit_menu");

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RES_FILE_MAT"))
				{
					IM_ASSERT(payload->DataSize == sizeof(std::string));
					std::string payload_n = *(std::string*)payload->Data;
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
		}
		else
		{
			ImGui::Text("Path"); ImGui::SameLine(0, ImGui::GetWindowWidth()*0.2f);
			if (ImGui::Button(rc->material->getMatPath().c_str()))
				ImGui::OpenPopup("mat_edit_menu");

			if (showMat && ImGui::Button("Collapse", ImVec2(ImGui::GetWindowWidth(), 0)))
			{
				showMat = false;
			}
			else if (!showMat && ImGui::Button("Edit", ImVec2(ImGui::GetWindowWidth(), 0)))
			{
				showMat = true;
			}

			if (showMat)
			{
				if (rc->material->type == SHADER_PBR)
				{
					BeginPreps("edit_mat34");
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
					PrepName("Normal Map Texture");
					PrepName("Metallic Texture");
					PrepName("Roughness Texture");
					PrepName("Emission Texture");

					NextPreps();
					ImGui::ColorEdit3("##AlbedoMat", (float*)&rc->material->albedo, ImGuiColorEditFlags_NoInputs);
					SetWeightPrep(); ImGui::DragFloat("##MetallicMat", &rc->material->metallic, 0.1f, 1);
					SetWeightPrep(); ImGui::DragFloat("##RoughnessMat", &rc->material->roughness, 0.1f, 1);
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
			}
		}
		ImGui::Separator();

		if (ImGui::BeginPopup("mat_edit_menu"))
		{
			if (ImGui::Selectable("Default-Material"))
				rc->material = rndr->materials.GetMaterialN("");
			if (ImGui::Selectable("Create"))
				ImGuiFileDialog::Instance()->OpenModal("CreateMatModel", "New Material", ".mat\0.mat\0.mat\0\0", ".mat");
			if (ImGui::Selectable("Load"))
				ImGuiFileDialog::Instance()->OpenModal("ChooseMatModel", "Select Material", ".mat\0.mat\0.mat\0\0", ".");
			if (!rc->material->isDefault && ImGui::Selectable("Save"))
			{
				cout << "OK : " << rc->material->getMatName() << endl;
				rndr->SaveMaterial(rc->material, rc->material->getMatName().c_str());
			}
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

				cout << "loading material : " << filePathName.c_str() << endl;
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
// -----------------------------------------------------------------------
void EditorProperty::dir_light_editor(Entity &obj)
{
	DrawComponent<DirectionalLight>("Directional Light", &obj,
		// Draw Properties
		[](Entity* ent, DirectionalLight* dir_light, bool removeComponent)
	{
		BeginPreps("edit_dl");
		PrepName("Intensity");
		PrepName("Color");
		PrepName("Cast Shadows");
		PrepName("Soft Shadows");
		PrepName("Bais");
		NextPreps();
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
		PrepName("Intensity");
		PrepName("Color");
		PrepName("Raduis");
		PrepName("Cast Shadows");
		PrepName("Bias");
		NextPreps();
		ImGui::DragFloat("##PLight_Intensity", &point_light->Intensity, 0.01f);
		SetWeightPrep();
		ImGui::ColorEdit3("##PLight_Color", (float*)&point_light->Color, ImGuiColorEditFlags_NoInputs);

		SetWeightPrep(); ImGui::DragFloat("##PLight_Linear", &point_light->Raduis, 0.01f);
		ImGui::Checkbox("##ppLight_active", &point_light->CastShadows);
		SetWeightPrep(); ImGui::DragFloat("##PLight_bias", &point_light->Bias, 0.01f);
		EndPreps();

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
		PrepName("Intensity");
		PrepName("Color");
		PrepName("Raduis");
		PrepName("Cut Off");
		PrepName("Outer Cut Off");
		PrepName("Cast Shadows");
		PrepName("Bias");
		NextPreps();
		ImGui::DragFloat("##PLight_Intensity", &spot_light->Intensity, 0.01f);
		SetWeightPrep();
		ImGui::ColorEdit3("##PLight_Color", (float*)&spot_light->Color, ImGuiColorEditFlags_NoInputs);

		SetWeightPrep(); ImGui::DragFloat("##PLight_rLinear", &spot_light->Raduis, 0.01f);
		SetWeightPrep(); ImGui::DragFloat("##PLight_rad", &spot_light->CutOff, 0.1f);
		SetWeightPrep(); ImGui::DragFloat("##PLight_orad", &spot_light->OuterCutOff, 0.1f);

		ImGui::Checkbox("##ssppLight_active", &spot_light->CastShadows);
		SetWeightPrep(); ImGui::DragFloat("##ssPLight_bias", &spot_light->Bias, 0.01f);
		ImGui::DragInt("##PLight_rLiaanear", (int*)&spot_light->shadow_index);
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
		PrepName("Box Projection");
		PrepName("Box");
		PrepName("");
		PrepName("Resolution");
		NextPreps();
		ImGui::DragFloat("##rp_Intensity", &ref_probe->Intensity, 0.01f);
		ImGui::Checkbox("##prpLight_sos", &ref_probe->static_only);
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
	bool remove = DrawComponent<RigidBody>("Rigid Body", &obj,
		// Draw Properties
		[](Entity* ent, RigidBody* rigidbody, bool removeComponent)
	{
		BeginPreps("edit_rbody");
		PrepName("Mode");
		PrepName("Mass");
		PrepName("Liner Damping");
		PrepName("Anguler Damping");
		NextPreps();

		int rbMode = 0;
		if (rigidbody->m_BodyMode == RB_STATIC)
			rbMode = 1;
		else if (rigidbody->m_BodyMode == RB_CHARACTAR)
			rbMode = 2;
		int seted_mode = rbMode;
		ImGui::Combo("##modeds_type", &seted_mode, "Rigid\0Static\0Charactar");
		if (rbMode != seted_mode) // Mode changed
		{
			rigidbody->SetBodyMode((RPhyBodyMode)seted_mode);
		}

		float mass = rigidbody->Mass;
		float ld = rigidbody->LinerDamping;
		float ad = rigidbody->AngulerDamping;
		SetWeightPrep(); ImGui::DragFloat("##rb_mass", &mass, 0.1f);
		SetWeightPrep(); ImGui::DragFloat("##rb_ld", &ld, 0.1f);
		SetWeightPrep(); ImGui::DragFloat("##rb_ad", &ad, 0.1f);
		rigidbody->SetMass(mass);
		rigidbody->SetDamping(ld, ad);
		EndPreps();

		ImGui::Button("Collision", ImVec2(ImGui::GetWindowWidth(), 0));
		BeginPreps("edit_rbodyColl");
		PrepName("Shape");
		PrepName("Offset");
		PrepName("Is Trigger");
		if (rigidbody->m_CollisionType == SPHERE_COLLIDER)
			PrepName("Raduis");
		else if (rigidbody->m_CollisionType == CAPSULE_COLLIDER)
			PrepName("Raduis | Height");
		else if (rigidbody->m_CollisionType == BOX_COLLIDER)
			PrepName("Scale");
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
		bool is_t = rigidbody->is_trigger;
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
		EndPreps();
	});

	if (remove)
		obj.RemoveComponent<RigidBody>();
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
			obj.GetComponent<ParticleSystem>()->mTexture = res->CreateTexture(filePathName, filePathName.c_str());
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
		PrepName("Looping");
		PrepName("Play on Start");
		PrepName("Volume");
		PrepName("Min Distance");
		PrepName("Max Distance");
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
			obj.GetComponent<GrassComponent>()->mTexture = res->CreateTexture(filePathName, filePathName.c_str());
		}
		ImGuiFileDialog::Instance()->CloseDialog("SetGrassTex");
	}
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
				ImGui::Text("4"); ImGui::SameLine();
				if (ImGui::Button("3")) // Save Prefab
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
		ImGui::SameLine(0, ImGui::GetWindowWidth() / 2 - 120);
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
// -----------------------------------------------------------------------