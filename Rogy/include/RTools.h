#include<GL\glew.h>
#include <stb_image.h>
#include <iostream>
#include <vector>
#include <string>

#include "../Engine/camera/Camera.h"

#include <glm/gtc/type_ptr.hpp>
#include "ImGuizmo.h"

#include <GLFW\glfw3.h>

#ifndef RTOOLS_H
#define RTOOLS_H

#define GCP_PATH_MAX 360

std::string RGetCurrentPath();
void RGetRelativePath(std::string& path);

void StrReplaceAll(std::string& str, const std::string& oldStr, const std::string& newStr);

void EditGizmos(glm::mat4 matr, glm::vec3 &pos, glm::vec3 &Scale, glm::vec3 &rot, glm::mat4 view, glm::mat4 proj, ImGuizmo::OPERATION mCurrentGizmoOperation, ImGuizmo::MODE mCurrentGizmoMode);

glm::vec3 from_float4(float * vals);

void renderSphere();

void renderQuad();

void renderCube();

void renderCube2();

void LoadCubemap(std::vector<std::string> faces, GLuint &textureID);

unsigned int loadTexture(char const * path);

void eraseSubStr(std::string& mainStr, std::string& SubStr);

void eraseAllSubStr(std::string& mainStr, std::string& SubStr);

namespace RTools
{
	bool CreateDir(const std::string& vPath);
	bool CreateDirectoryIfNotExist(const std::string& name);
	bool IsDirectoryExist(const std::string& name);
};

#endif // RTOOLS_H