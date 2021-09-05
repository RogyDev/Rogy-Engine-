#include "material_editor.h"

RMaterialEditor::RMaterialEditor()
{
}

RMaterialEditor::~RMaterialEditor()
{
}

void RMaterialEditor::EditMaterial(std::string& mat_source)
{
	isOn = true;
	mat = renderer->LoadMaterial(mat_source.c_str());
}

void RMaterialEditor::EditMaterial(Material* mat_source)
{
	isOn = true;
	mat = mat_source;
}

void RMaterialEditor::SaveMaterial()
{
	renderer->SaveMaterial(mat, mat->getMatName().c_str());
}

void RMaterialEditor::Render()
{
	if (!isOn) return;

	ImGui::Begin("Material Editor", &isOn, ImGuiWindowFlags_NoCollapse);
	if (mat == nullptr) 
	{ 
		ImGui::Text("");
		ImGui::Text("");
		ImGui::SameLine(0, ImGui::GetWindowWidth() / 2 - 70);
		ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 0.6f), "No Material Selected.");

		ImGui::End();
		return; 
	}
	if (ImGui::Button("Save", ImVec2(ImGui::GetWindowWidth(), 0)))
		renderer->SaveMaterial(mat, mat->getMatName().c_str());

	if (mat->type == SHADER_PBR)
	{
		// ---------------------- Properties ------------------------------//
		ImGui::Button("Properties", ImVec2(ImGui::GetWindowWidth(), 0));
		ImGui::Columns(2, "mat_edit_props", false);
		ImGui::SetColumnWidth(0, ImGui::GetWindowWidth()*0.4f);
		PrepName("Type");
		PrepName("TAG");
		PrepName("Ambient");
		PrepName("UV");
		PrepName("Cutout");
		ImGui::NextColumn();
		int tp = (int)mat->type;
		SetWeightPrep(); ImGui::Combo("#mat_type", &tp, "Default\0Blend");
		mat->type = (Shader_Type)tp;
		SetWeightPrep(); ImGui::InputText("##mat_tag", &mat->tag);
		SetWeightPrep(); ImGui::DragFloat("##AONat", &mat->ao, 0.01f, 0, 1, "%.3g");
		SetWeightPrep(); ImGui::DragFloat2("##uvNat", (float*)&mat->uv, 0.01f);
		ImGui::Checkbox("##cutUsage", &mat->cutout);
		ImGui::Columns(1);
		// ---------------------- Color ------------------------------//
		ImGui::Button("Color", ImVec2(ImGui::GetWindowWidth(), 0));
		ImGui::Columns(2, "mat_edit_Color", false);
		ImGui::SetColumnWidth(0, ImGui::GetWindowWidth()*0.4f);
		PrepName("Color");
		PrepName("Texture");
		ImGui::Dummy(ImVec2(0, 30.0f));
		ImGui::NextColumn();
		ImGui::ColorEdit3("##AlbedoMat", (float*)&mat->albedo, ImGuiColorEditFlags_NoInputs);
		EditTextureID(mat, 0);
		ImGui::Columns(1);
		// ---------------------- Normal ------------------------------//
		ImGui::Button("Normal", ImVec2(ImGui::GetWindowWidth(), 0));
		ImGui::Columns(2, "mat_edit_Normal", false);
		ImGui::SetColumnWidth(0, ImGui::GetWindowWidth()*0.4f);
		PrepName("Texture");
		ImGui::Dummy(ImVec2(0, 30.0f));
		ImGui::NextColumn();
		EditTextureID(mat, 1);
		ImGui::Columns(1);
		// ---------------------- Roughness ------------------------------//
		ImGui::Button("Roughness", ImVec2(ImGui::GetWindowWidth(), 0));
		ImGui::Columns(2, "mat_edit_Roughness", false);
		ImGui::SetColumnWidth(0, ImGui::GetWindowWidth()*0.4f);
		PrepName("Roughness");
		PrepName("Texture");
		ImGui::Dummy(ImVec2(0, 30.0f));
		ImGui::NextColumn();
		SetWeightPrep(); ImGui::DragFloat("##RoughnessMat", &mat->roughness, 0.03f, 0.0f, 1.0f);
		EditTextureID(mat, 3);
		ImGui::Columns(1);
		// ---------------------- Metallic ------------------------------//
		ImGui::Button("Metallness", ImVec2(ImGui::GetWindowWidth(), 0));
		ImGui::Columns(2, "mat_edit_Metallic", false);
		ImGui::SetColumnWidth(0, ImGui::GetWindowWidth()*0.4f);
		PrepName("Metallness");
		PrepName("Texture");
		ImGui::Dummy(ImVec2(0, 30.0f));
		ImGui::NextColumn();
		SetWeightPrep(); ImGui::DragFloat("##MetallicMat", &mat->metallic, 0.03f, 0.0f, 1.0f);
		EditTextureID(mat, 2);
		ImGui::Columns(1);
		// ---------------------- Emission ------------------------------//
		ImGui::Button("Emission", ImVec2(ImGui::GetWindowWidth(), 0));
		ImGui::Columns(2, "mat_edit_Emission", false);
		ImGui::SetColumnWidth(0, ImGui::GetWindowWidth()*0.4f);
		PrepName("Enable");
		PrepName("Color");
		PrepName("Power");
		PrepName("Texture");
		ImGui::Dummy(ImVec2(0, 30.0f));
		ImGui::NextColumn();
		ImGui::Checkbox("##emmUsage", &mat->use_emission);
		SetWeightPrep();
		ImGui::ColorEdit3("##emmMat", (float*)&mat->emission, ImGuiColorEditFlags_NoInputs);
		SetWeightPrep(); ImGui::DragFloat("##emmMatpwr", &mat->emission_power, 0.03f, 0.0f);
		EditTextureID(mat, 4);
		ImGui::Columns(1);
	}
	else if(mat->type == SHADER_Blend)
	{
		// ---------------------- Properties ------------------------------//
		ImGui::Button("Properties", ImVec2(ImGui::GetWindowWidth(), 0));
		ImGui::Columns(2, "mat_edit_props", false);
		ImGui::SetColumnWidth(0, ImGui::GetWindowWidth()*0.4f);
		PrepName("Type");
		PrepName("TAG");
		PrepName("Ambient");
		PrepName("UV");
		ImGui::NextColumn();
		int tp = (int)mat->type;
		SetWeightPrep(); ImGui::Combo("#mat_type", &tp, "Default\0Blend");
		mat->type = (Shader_Type)tp;
		SetWeightPrep(); ImGui::InputText("##mat_tag", &mat->tag);
		SetWeightPrep(); ImGui::DragFloat("##AONat", &mat->ao, 0.01f, 0, 1, "%.3g");
		SetWeightPrep(); ImGui::DragFloat2("##uvNat", (float*)&mat->uv, 0.01f);
		ImGui::Columns(1);
		// ---------------------- Material 01 ------------------------------//
		ImGui::Button("Material 01", ImVec2(ImGui::GetWindowWidth(), 0));
		ImGui::Columns(2, "mat_edit_Color", false);
		ImGui::SetColumnWidth(0, ImGui::GetWindowWidth()*0.4f);
		PrepName("Color");
		PrepName("Roughness");
		PrepName("Metallness");
		PrepName("Albedo");
		ImGui::Dummy(ImVec2(0, 30.0f));
		PrepName("Normal");
		ImGui::Dummy(ImVec2(0, 30.0f));
		PrepName("Roughness");
		ImGui::Dummy(ImVec2(0, 30.0f));
		ImGui::NextColumn();
		ImGui::ColorEdit3("##AlbedoMat", (float*)&mat->albedo, ImGuiColorEditFlags_NoInputs);
		SetWeightPrep(); ImGui::DragFloat("##RoughnessMat", &mat->roughness, 0.03f, 0.0f, 1.0f);
		SetWeightPrep(); ImGui::DragFloat("##MetallnessMat", &mat->metallic, 0.03f, 0.0f, 1.0f);
		EditTextureID(mat, 0);
		EditTextureID(mat, 1);
		EditTextureID(mat, 3);
		ImGui::Columns(1);
		// ---------------------- Material 02 ------------------------------//
		ImGui::Button("Material 02", ImVec2(ImGui::GetWindowWidth(), 0));
		ImGui::Columns(2, "mat_edit_Color2", false);
		ImGui::SetColumnWidth(0, ImGui::GetWindowWidth()*0.4f);
		PrepName("Color");
		PrepName("Roughness");
		PrepName("Metallness");
		PrepName("Albedo");
		ImGui::Dummy(ImVec2(0, 30.0f));
		PrepName("Normal");
		ImGui::Dummy(ImVec2(0, 30.0f));
		ImGui::NextColumn();
		ImGui::ColorEdit3("##AlbedoMat2", (float*)&mat->emission, ImGuiColorEditFlags_NoInputs);
		SetWeightPrep(); ImGui::DragFloat("##RoughnessMat2", &mat->val1, 0.03f, 0.0f, 1.0f);
		SetWeightPrep(); ImGui::DragFloat("##MetallnessMat2", &mat->val2, 0.03f, 0.0f, 1.0f);
		EditTextureID(mat, 2);
		EditTextureID(mat, 4);
		ImGui::Columns(1);
	}

	/*if (hasTex)	DisplayInfo(mat->tex_emission->getTexName().c_str());
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
			mat->tex_emission = res->CreateTexture(payload_n.substr(slashPos + 1, std::string::npos), payload_n.c_str());
		}
		ImGui::EndDragDropTarget();
	}
	if (hasTex) {
		ImGui::SameLine();
		if (ImGui::Button("X ##emmT"))
			mat->tex_emission = nullptr;
	}*/

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
				mat->tex_albedo = res->CreateTexture(fileName, filePathName.c_str());
			if (tex_index == 2)
				mat->tex_normal = res->CreateTexture(fileName, filePathName.c_str());
			if (tex_index == 3)
				mat->tex_metal = res->CreateTexture(fileName, filePathName.c_str());
			if (tex_index == 4)
				mat->tex_rough = res->CreateTexture(fileName, filePathName.c_str());
			if (tex_index == 5)
				mat->tex_emission = res->CreateTexture(fileName, filePathName.c_str());
		}
		// close
		ImGuiFileDialog::Instance()->CloseDialog("ChooseTexModel");
	}
	ImGui::End();
}

void RMaterialEditor::OpenTextureSelect()
{
	ImGuiFileDialog::Instance()->OpenModal("ChooseTexModel", "Choose Texture", ".png\0.bmp\0.tga\0.PNG\0.BMP\0.TGA\0.jpg\0.JPG\0.psd\0.tif", ".");
}

void RMaterialEditor::DisplayInfo(const char* desc)
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

void RMaterialEditor::EditTextureID(Material* tex, int indx)
{
	hasTex = false;
	if (indx == 0)
	{
		ImGui::PushID("SET_ALBEDO_TEXTURE");
		bool hasTex = (tex->tex_albedo != nullptr);
		if ((hasTex && ImGui::ImageButton((ImTextureID)tex->tex_albedo->getTexID(), ImVec2(50.f, 50.f)))
			|| (!hasTex && ImGui::Button("(None)", ImVec2(50.f, 50.f))))
		{
			OpenTextureSelect();
			tex_index = 1;
		}
		ImGui::PopID();
		if (hasTex)	DisplayInfo(tex->tex_albedo->getTexName().c_str());
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
	}
	else if (indx == 1)
	{
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
	}
	else if (indx == 2)
	{
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
	}
	else if (indx == 3)
	{
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
	}
	else if (indx == 4)
	{
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
	}
}

void RMaterialEditor::EditTextureProprty(Material* tex)
{
	//static int tex_index = 0;
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
	if (hasTex)	DisplayInfo(tex->tex_albedo->getTexName().c_str());
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