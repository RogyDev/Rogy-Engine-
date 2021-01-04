#ifndef SCENE_SETTINGS_H
#define SCENE_SETTINGS_H

#include <GL\glew.h>

#include <iostream>
#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "imgui_stdlib.h"
#include <glm\glm.hpp>
#include "../scene/Scene.h"
#include "../renderer/renderer.h"
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <GLFW\glfw3.h>

class Scene_Settings
{
public:
	Scene_Settings();
	~Scene_Settings();

	Renderer* rndr = nullptr;

	void Render();

private:

};

#endif // SCENE_SETTINGS_H