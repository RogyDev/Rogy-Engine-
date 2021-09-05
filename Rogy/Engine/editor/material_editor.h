#ifndef RMATERIAL_EDITOR_H
#define RMATERIAL_EDITOR_H

#include <iostream>
#include <string>

#include "imgui.h"
#include "imgui_stdlib.h"
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include "../renderer/renderer.h"

#include <RTools.h>

class RMaterialEditor
{
public:
	RMaterialEditor();
	~RMaterialEditor();

	Renderer* renderer;
	ResourcesManager* res;

	void EditMaterial(Material* mat_source);
	void EditMaterial(std::string &mat_source);
	void SaveMaterial();
	void Render();

	void OpenTextureSelect();
	void DisplayInfo(const char* desc);
	void EditTextureProprty(Material* tex);
	void EditTextureID(Material* tex, int indx);

	void PrepName(const char* p_name)
	{
		ImGui::Dummy(ImVec2(0, 2.0f));
		ImGui::Text(p_name);
	}
	void SetWeightPrep()
	{
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 1.8f);
	}

	std::string CurrentMatSource;
	Material* mat = nullptr;
	bool isOn = false;

private:
	int tex_index = 0;
	bool hasTex = false;
};

#endif // EDITOR_PROJECT_BROWSER_H