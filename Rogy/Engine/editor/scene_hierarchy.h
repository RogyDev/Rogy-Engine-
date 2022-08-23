#ifndef SCENE_HIER_H
#define SCENE_HIER_H

#include <iostream>
#include <string>
#include <GL\glew.h>

#include "../shading/viewportBounds.h"
#include <glm\glm.hpp>
#include "../scene/Scene.h"
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "../input/InputManager.h"

class Scene_hierarchy
{
public:
	Scene_hierarchy();
	~Scene_hierarchy();

	ImFont* icon_small;

	std::vector<EnttID> sel_entt;
	std::vector<EnttID> auto_open_entt;

	void SetSelection(EnttID eid, bool diselect_others = true, bool deselectIfSelected = false);

	SceneManager* scene;
	InputManager* input = nullptr;

	// Checks if the selected entities list is empty or not.
	bool Empty();

	EnttID GetSelected();
	bool IsSelected(EnttID eid);

	void LoopInChild(Entity* entit);

	void Render();

	bool TreeNodeExV(const void* ptr_id, ImGuiTreeNodeFlags flags, const char* fmt);
	void SaveACopy();
	void PastACopy();
	bool isOn = true;
private:
	bool  ent_ed_popup = false;
	EnttID copyParentID;
};

#endif