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
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

class Scene_hierarchy
{
public:
	Scene_hierarchy();
	~Scene_hierarchy();

	ImFont* icon_small;

	vector<EnttID> sel_entt;

	SceneManager* scene;

	// Checks if the selected entities list is empty or not.
	bool Empty();

	EnttID GetSelected();

	void LoopInChild(Entity* entit);

	void Render();

	void SaveACopy();
	void PastACopy();
private:
	bool  ent_ed_popup = false;
};

#endif