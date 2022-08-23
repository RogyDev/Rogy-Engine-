#ifndef R_EDITOR_PROP
#define R_EDITOR_PROP

#include "dirent.h"
#include <GL\glew.h>

#include <iostream>
#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "imgui_stdlib.h"
#include "../shading/viewportBounds.h"

#include <glm\glm.hpp>
#include "scene_hierarchy.h"
#include "scene_settings.h"
#include <ImGuiFileDialog\ImGuiFileDialog.h>
#include "script_editor.h"
#include "debug_editor.h"
#include "project_browser.h"

#include "editor_property.h"
#include "project_settings.h"
#include "material_editor.h"

class Game_Editor 
{
public:
	Game_Editor();
	~Game_Editor();
	
	bool scripts_board = false;
	bool tryingtoSpawn = false;

	std::string ProjectResourcesFolder;

	ViewPort_Bounds* MainViewport;
	Scene_hierarchy s_hierarchy;
	EditorProperty prep_editor;
	Scene_Settings scn_settings;
	Script_Editor code_editor;
	Editor_Debug db_editor;
	EProjectBrowser prj_browser;
	Project_Settings prg_settings;
	RMaterialEditor mat_editor;

	ImGuizmo::OPERATION mCurrentGizmoOperation;
	ImGuizmo::MODE mCurrentGizmoMode;
	float snaping = 0.00f;
	bool BoundSizing = false;

	int SCR_weight, SCR_height;

	ImGuiViewport *viewport;

	bool Init(char* glsl_version, GLFWwindow* window, bool platfrm_support);
	
	bool isMouseInEditor();

	void start();

	void render();

	bool BeginWindow(char* name);
	void EndWindow();
	void BeginPreps(char* nm);
	void PrepName(char* p_name);
	void NextPreps();
	void EndPreps();
	void ShutDown();
	void EditVec3_xyz(const char* name_id, glm::vec3 &values);

	void ToolBar();
	void DownBar();
	void DockSpaces();

	void CreateEntityWithMesh(std::string ent_name, std::string mesh_path, const char* col_type);

private:
	bool platformSupport = false;
};

#endif