#ifndef EDITOR_PROJECT_MANAGER_H
#define EDITOR_PROJECT_MANAGER_H

#include <iostream>
#include <string>

#include "imgui.h"
#include "imgui_stdlib.h"
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <dirent.h>

class ProjectManager
{
public:
	ProjectManager();
	~ProjectManager();

	void Init(std::string engine_directory);
	void Render();

	bool CreateNewProject();
	void OpenProject(std::string dir);
	void ScanDirForProjects(std::string dir);
	bool IsDirectoryExist(const std::string& name);
	bool CreateDirectoryIfNotExist(const std::string& name);

	bool isOn = false;
	ImTextureID logoID;

private:
	bool initialized = false;
	std::string engine_dir;
	std::string ProjectName;
	std::string scrfilter;
	std::vector<std::string> ProjectsList;
	bool createNew = false;
	void startupProject();
};


#endif // EDITOR_PROJECT_MANAGER_H