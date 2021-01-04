#ifndef EDITOR_PROJECT_BROWSER_H
#define EDITOR_PROJECT_BROWSER_H

#include <iostream>
#include <string>

#include "imgui.h"
#include "imgui_stdlib.h"
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <dirent.h>

// For opennig scripts
#include "script_editor.h"

#include <RTools.h>

struct EFileInfo
{
	std::string name;
	std::string name_only;
	std::string file_type;
	bool is_dir;
	bool is_selected;
	bool is_texture;

	EFileInfo(std::string _name, std::string _name_only, std::string _file_type, bool _is_dir)
	{
		name = _name;
		name_only = _name_only;
		file_type = _file_type;
		is_dir = _is_dir;
		is_selected = false;
		is_texture = false;
	}
};

class EProjectBrowser
{
public:
	EProjectBrowser();
	~EProjectBrowser();

	std::string res_dir;
	std::string current_dir;
	std::string shortcurrent_dir;
	std::string folder_name, scr_name;
	std::string default_Script;
	std::vector<std::string> last_copied;
	bool last_operation_is_cut = false;

	Script_Editor* scr_editor;

	bool isOn = true;

	std::vector<EFileInfo> dir_files;

	void Render(ImFont* icon_font = nullptr);

	void OpenDir(const char* path);
	void GoBack();

	bool IsDirectoryExist(const std::string& name);
	bool CreateDirectoryIfNotExist(const std::string& name);
	bool CreateDir(const std::string& vPath);

	void DisplayInfo(const char* desc);

private:
	bool initialized = false;
};


#endif // EDITOR_PROJECT_BROWSER_H