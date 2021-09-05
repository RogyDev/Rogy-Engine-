#ifndef R_DEBUG_EDITOR_H
#define R_DEBUG_EDITOR_H

#include <iostream>
#include <string>

#include "imgui.h"
#include "imgui_stdlib.h"
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include "../io/debug_tools.h"

class Editor_Debug
{
public:
	Editor_Debug();
	~Editor_Debug();

	DebugTool* debuger;
	
	bool is_open = false;
	void Render();
private:

};

#endif // RSCRIPT_EDITOR_H