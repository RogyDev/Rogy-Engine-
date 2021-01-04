#ifndef RSCRIPT_EDITOR_H
#define RSCRIPT_EDITOR_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <RTools.h>

#include "imgui.h"
#include "imgui_stdlib.h"
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include "TextEditor.h"

struct EditedScriptInfo
{
	TextEditor t_editor;
	std::string path;
	std::string name;
	std::string code;
	bool used;
};

class Script_Editor
{
public:
	Script_Editor();
	~Script_Editor();

	bool isOn = false;
	std::vector<EditedScriptInfo> m_OpenedScripts;

	void OpenScript(std::string path, std::string nme = "Lua Script");
	void CloseScript(std::string path);
	void SaveScript(std::string path);

	void Render(ImFont* code_font = nullptr);

private:

};



#endif // RSCRIPT_EDITOR_H