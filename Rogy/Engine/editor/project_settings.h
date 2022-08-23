#ifndef EDITOR_PROJECT_SETTINGS_H
#define EDITOR_PROJECT_SETTINGS_H


#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <GL\glew.h>

#include <iostream>
#include <string>

#include "../ProjectSettings.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../renderer/renderer.h"
#include "../input/InputManager.h"


#include "editor_property.h"

class Project_Settings
{
public:
	Project_Settings();
	~Project_Settings();

	Renderer* rndr = nullptr;
	RProjectSettings* prj = nullptr;
	InputManager* input = nullptr;
	//UIRenderer* ui = nullptr;
	std::string loadFont;
	int lfs = 50;

	bool isOn = false;

	void Render();

private:
	int renameAxisIndex = 0;
	std::string aNewAxis = "AxisName";
	std::string aNewfont = "";
	int aNewfontSize = 50;
	int firstKey, secondKey;
	bool editfirstKey = false, editsecondKey = false;
};

#endif // EDITOR_PROJECT_SETTINGS_H