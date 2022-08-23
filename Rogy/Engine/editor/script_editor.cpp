#include "script_editor.h"
// --------------------------------------------------------
Script_Editor::Script_Editor()
{
}
// --------------------------------------------------------
Script_Editor::~Script_Editor()
{
}
// --------------------------------------------------------
void Script_Editor::OpenScript(std::string path, std::string nme)
{
	std::filebuf fb;
	if (!fb.open(path, std::ios::in))
		return;
	fb.close();
	scriptOpend = true;

	// Load script from path
	std::ifstream infile(path);
	std::stringstream strs;
	strs << infile.rdbuf();
	std::string str = strs.str();
	infile.close();

	EditedScriptInfo script_info;
	script_info.path = path;
	script_info.code = str;
	script_info.name = nme;
	script_info.used = false;
	script_info.t_editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
	script_info.t_editor.SetText(script_info.code);
	script_info.t_editor.SetPalette(script_info.t_editor.GetDarkPalette()) ;
	
	m_OpenedScripts.push_back(script_info);
}
// --------------------------------------------------------
void Script_Editor::CloseScript(std::string path)
{
	for (size_t i = 0; i < m_OpenedScripts.size(); i++)
	{
		if (m_OpenedScripts[i].path == path)
		{
			m_OpenedScripts.erase(m_OpenedScripts.begin() + i);
		}
	}
}
// --------------------------------------------------------
void Script_Editor::SaveScript(std::string path)
{
	for (size_t i = 0; i < m_OpenedScripts.size(); i++)
	{
		if (m_OpenedScripts[i].path == path)
		{
			m_OpenedScripts[i].code = m_OpenedScripts[i].t_editor.GetText();
			std::ofstream os(path, std::ios::binary);
			os << m_OpenedScripts[i].code;
			os.close();
		}
	}
}
// --------------------------------------------------------
void Script_Editor::Render(ImFont* code_font)
{
	if (!isOn) return;
	std::string win_title;
	for (size_t i = 0; i < m_OpenedScripts.size(); i++)
	{
		bool is_on = true;
		win_title = m_OpenedScripts[i].name;
		win_title += "##";
		win_title += m_OpenedScripts[i].path;
		ImGui::Begin(win_title.c_str(), &is_on, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
		
		if (!is_on) CloseScript(m_OpenedScripts[i].path);
		
		TextEditor& t_editor = m_OpenedScripts[i].t_editor;
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save"))
				{
					SaveScript(m_OpenedScripts[i].path);
				}
				if (ImGui::MenuItem("Open.."))
				{
					ImGuiFileDialog::Instance()->OpenModal("LoadScrModel", "Open Script", ".lua\0.LUA\0\0", ".");
				}
				if (ImGui::MenuItem("Close"))
				{
					CloseScript(m_OpenedScripts[i].path);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "Ctrl+Z"))
					t_editor.Undo();
				if (ImGui::MenuItem("Redo", "Ctrl+Y"))
					t_editor.Redo();
				ImGui::Separator();
				if (ImGui::MenuItem("Copy", "Ctrl+C"))
					t_editor.Copy();
				if (ImGui::MenuItem("Cut", "Ctrl+X"))
					t_editor.Cut();
				if (ImGui::MenuItem("Past", "Ctrl+V"))
					t_editor.Paste();
				ImGui::EndMenu();
			}
			if (m_OpenedScripts[i].t_editor.GetText() != m_OpenedScripts[i].code)
			{
				ImGui::Text("| (*) edited");
				if (ImGui::MenuItem("Save"))
				{
					SaveScript(m_OpenedScripts[i].path);
				}
			}
			
			ImGui::EndMenuBar();
		}

		if (code_font != nullptr)
		{
			ImGui::SetWindowFontScale(0.95f);
			ImGui::PushFont(code_font);
		}

		t_editor.Render("code_editer");
		if (ImGui::IsItemClicked(1))
			ImGui::OpenPopup("editM_popup");

		if (code_font != nullptr)
			ImGui::PopFont();

		if (ImGui::BeginPopup("editM_popup"))
		{
			if (ImGui::MenuItem("Undo", "Ctrl+Z"))
				t_editor.Undo();
			if (ImGui::MenuItem("Redo", "Ctrl+Y"))
				t_editor.Redo();
			ImGui::Separator();
			if (ImGui::MenuItem("Copy", "Ctrl+C"))
				t_editor.Copy();
			if (ImGui::MenuItem("Cut", "Ctrl+X"))
				t_editor.Cut();
			if (ImGui::MenuItem("Past", "Ctrl+V"))
				t_editor.Paste();

			ImGui::EndPopup();
		}
		ImGui::End();
	}

	if (ImGuiFileDialog::Instance()->FileDialog("LoadScrModel"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk == true)
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			filePath += "\\" ;
			std::string fileName = filePathName;
			eraseSubStr(fileName, filePath);

			OpenScript(filePathName, fileName);
		}
		// close
		ImGuiFileDialog::Instance()->CloseDialog("LoadScrModel");
	}
}
// --------------------------------------------------------