#include "project_browser.h"
#include <experimental\filesystem>

#include "imgui_internal.h"
#include "imgui_stdlib.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// --------------------------------------------------------
EProjectBrowser::EProjectBrowser()
{
}
// --------------------------------------------------------
EProjectBrowser::~EProjectBrowser()
{
	// TODO delete textures of "icons"
}
// --------------------------------------------------------
void EProjectBrowser::DisplayInfo(const char* desc, const char* desc2)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::Separator();
		ImGui::NewLine();
		ImGui::TextUnformatted(desc2);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}
int EProjectBrowser::GetSelectionIndex()
{
	for (size_t i = 0; i < dir_files.size(); i++)
	{
		if (dir_files[i].is_selected)
			return i;
	}
	return -1;
}
// --------------------------------------------------------
/*EFileInfo & EProjectBrowser::GetSelection()
{
	for (size_t i = 0; i < dir_files.size(); i++)
	{
		if (dir_files[i].is_selected)
			return dir_files[i];
	}
	return EFileInfo();
}*/
// --------------------------------------------------------
int no_directories(const struct dirent *entry)
{
	int pass;

	if (entry->d_type != DT_DIR) {
		pass = 1;
	}
	else {
		pass = 0;
	}

	return pass;
}
// ----------------------------------------------------
void EProjectBrowser::Init()
{
	icons["folder"] = new Texture();
	icons["folder"]->setTexture("core\\editor\\DirectoryIcon.png", "folder");

	icons["mat"] = new Texture();
	icons["mat"]->setTexture("core\\editor\\Material.png", "mat");

	icons["lua"] = new Texture();
	icons["lua"]->setTexture("core\\editor\\Script.png", "lua");

	icons["pf"] = new Texture();
	icons["pf"]->setTexture("core\\editor\\Prefab.png", "pf");

	icons["scn"] = new Texture();
	icons["scn"]->setTexture("core\\editor\\Scene.png", "scn");

	icons["tex"] = new Texture();
	icons["tex"]->setTexture("core\\editor\\Texture.png", "tex");

	icons["audio"] = new Texture();
	icons["audio"]->setTexture("core\\editor\\Audio.png", "audio");

	icons["file"] = new Texture();
	icons["file"]->setTexture("core\\editor\\FileIcon.png", "file");

	current_dir = RGetCurrentPath();
	res_dir = current_dir + "\\";
	res_dir += ProjectDir;
	folder_name = "New Folder";
	scr_name = "NewScript";
	mat_name = "NewMaterial";
	/*"function XXX: init()\n"
		"\t--self.Var = 1.0;\n"
		"\t--self._localVar = 1.0;\n"
		"end\n\n"*/
	default_Script = "XXX = ScriptComponent  \n"
		"{\n"
		"    Var = 1.0;\n"
		"    _localVar = 1.0;\n"
		"}\n\n"
		"-- OnStart called when the entity starts\n"
		"function XXX:OnStart()\n    \n"
		"end\n\n"
		"-- OnUpdate called every frame.\n"
		"function XXX:OnUpdate(dt)\n    \n"
		"end\n";

	OpenDir(res_dir.c_str());
}
// --------------------------------------------------------
void EProjectBrowser::Render(ImFont* icon_font)
{
	if (!initialized)
	{
		Init();
		initialized = true;
	}

	if (!isOn) return;

	ImGui::Begin("Project Browser", &isOn, ImGuiWindowFlags_MenuBar);
	
	if (ImGui::BeginMenuBar())
	{
		ImGui::Checkbox("Stick", &Stick);
		if (ImGui::MenuItem("<"))
			GoBack();
		ImGui::MenuItem(">"); // TODO
		if (ImGui::MenuItem("Refresh"))
			RefreshDir();

		static std::string scrfilter;
		ImGui::SetNextItemWidth(200.0f);
		ImGui::InputTextWithHint("##_scrfilter", "Search...", &scrfilter);

		ImGui::Text(shortcurrent_dir.c_str());
		ImGui::EndMenuBar();
	}
	ImGui::BeginChild("scn_Child_h", ImVec2(ImGui::GetWindowWidth()/12.0f, 0.0f));
	ImGui::Text(ProjectDir.c_str());
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.11f, 0.11f, 0.11f, 1.00f));
	ImGui::BeginChild("scn_Child");
	ImGui::NewLine();
	//ImGui::Separator();
	if (dir_files.empty())
	{
		ImGui::Text(""); ImGui::Text("");
		ImGui::SameLine(0, ImGui::GetWindowWidth() / 2 - 60);
		ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 0.6f), "This folder is empty");
	}
	size_t f = size_t(ImGui::GetWindowWidth() / 102);
	if (f < 1) f = 1;
	size_t lineCount = (dir_files.size() / f) + 1;
	if (lineCount < 1) f = 1;
	size_t foffset = 0;
	int thumbnail_index = 0;
	for (size_t j = 0; j < lineCount; j++)
	{
		ImGui::Columns(f, NULL, false);
		for (size_t i = foffset; i < foffset + f && i < dir_files.size(); i++)
		{
			bool selectble;
			bool has_thumbnail = false;
			if (icon_font != nullptr)
			{
				ImTextureID icn = (ImTextureID)icons["file"]->getTexID();

				//ImGui::PushFont(icon_font);
				std::string file_icon = "B"; // default
				if (dir_files[i].is_dir)
				{
					file_icon = "A";
					icn = (ImTextureID)icons["folder"]->getTexID();
				}
				else if (dir_files[i].file_type == "rbp") {
					file_icon = "4";
					dir_files[i].is_prefab = true;
					icn = (ImTextureID)icons["pf"]->getTexID();
				}
				else if (dir_files[i].file_type == "lua" || dir_files[i].file_type == "LUA")
				{
					file_icon = "y";
					icn = (ImTextureID)icons["lua"]->getTexID();
				}
				else if (dir_files[i].file_type == "mat")
				{
					file_icon = "m";
					icn = (ImTextureID)icons["mat"]->getTexID();
					if (thumbnail_index < thumbnails.size())
					{
						if(thumbnails[thumbnail_index].Loaded)
							icn = (ImTextureID)thumbnails[thumbnail_index].thumbnail;

						thumbnail_index++;
					}
				}
				else if (dir_files[i].file_type == "rscn")
				{
					file_icon = "{";
					icn = (ImTextureID)icons["scn"]->getTexID();
				}
				else if (dir_files[i].file_type == "obj" || dir_files[i].file_type == "fbx"
					|| dir_files[i].file_type == "OBJ" || dir_files[i].file_type == "FBX"
					|| dir_files[i].file_type == "glb" || dir_files[i].file_type == "gltf") {
					file_icon = "t";
					dir_files[i].is_mesh = true;
					icn = (ImTextureID)icons["pf"]->getTexID();
				}
				else if (dir_files[i].file_type == "wav" || dir_files[i].file_type == "mp3") {
					icn = (ImTextureID)icons["audio"]->getTexID();
				}
				else if (dir_files[i].file_type == "bmp" || dir_files[i].file_type == "png"
					|| dir_files[i].file_type == "hdr" || dir_files[i].file_type == "PNG"
					|| dir_files[i].file_type == "BMP" || dir_files[i].file_type == "HDR"
					|| dir_files[i].file_type == "jpg" || dir_files[i].file_type == "JPG")
				{
					file_icon = "a";
					dir_files[i].is_texture = true;
					icn = (ImTextureID)icons["tex"]->getTexID();
				}

				ImGui::PushID(i + 2);
				ImVec4 colr = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
				//if(file_icon == "A") colr = ImVec4(0.76f, 0.62f, 0.35f, 1.00f);
				 if (file_icon == "4")  colr = ImVec4(0.22f, 0.36f, 0.46f, 1.00f);

				//ImGui::PushStyleColor(ImGuiCol_Text, colr);
				selectble = ImageSelectable(dir_files[i].name_only.c_str(), icn, &dir_files[i].is_selected, NULL, ImVec2(90, 127), colr, file_icon.c_str());
				
				if (ImGui::IsItemHovered())
					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

				//selectble = ImGui::ImageButton((ImTextureID)icons["file"]->getTexID(), &dir_files[i].is_selected, NULL, ImVec2(65, 70));
				//ImGui::PopStyleColor();
				//ImGui::PopFont();
				if (!dir_files[i].is_dir && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					std::string drag_pathS = current_dir + "\\" + dir_files[i].name;
					RGetRelativePath(drag_pathS);
					const wchar_t* drag_path = (const wchar_t*)drag_pathS.c_str();
					if (dir_files[i].is_texture)
						ImGui::SetDragDropPayload("RES_FILE_TEX", &drag_pathS, sizeof(std::string));
					else if (dir_files[i].file_type == "mat")
						ImGui::SetDragDropPayload("RES_FILE_MAT", drag_path, (wcslen(drag_path) + 1), sizeof(wchar_t));
					else if (dir_files[i].file_type == "rbp")
						ImGui::SetDragDropPayload("RES_FILE_RBP", &drag_pathS, sizeof(std::string));
					else if (dir_files[i].file_type == "rscn")
						ImGui::SetDragDropPayload("RES_FILE_SCENE", &drag_pathS, sizeof(std::string));
					else if (dir_files[i].is_mesh)
						ImGui::SetDragDropPayload("RES_FILE_MESH", &drag_pathS, sizeof(std::string));
					ImGui::Text(drag_pathS.c_str());
					ImGui::EndDragDropSource();

					/*
					std::string drag_path = current_dir + "\\" + dir_files[i].name;
					RGetRelativePath(drag_path);
					const wchar_t* drag_path = (current_dir + "\\" + dir_files[i].name).c_str();
					if (dir_files[i].is_texture)
						ImGui::SetDragDropPayload("RES_FILE_TEX", &drag_path, sizeof(std::string));
					else if (dir_files[i].file_type == "mat")
						ImGui::SetDragDropPayload("RES_FILE_MAT", drag_path.c_str(), drag_path.size());
					else if (dir_files[i].file_type == "rbp")
						ImGui::SetDragDropPayload("RES_FILE_RBP", &drag_path, sizeof(std::string));
					else if (dir_files[i].file_type == "rscn")
						ImGui::SetDragDropPayload("RES_FILE_SCENE", &drag_path, sizeof(std::string));
					else if (dir_files[i].is_mesh)
						ImGui::SetDragDropPayload("RES_FILE_MESH", &drag_path, sizeof(std::string));
					ImGui::Text(drag_path.c_str());
					ImGui::EndDragDropSource();*/
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
				{
					std::string p = current_dir + "\\" + dir_files[i].name;
					RGetRelativePath(p);
					// Enter directory
					if (dir_files[i].is_dir)
						OpenDir((current_dir + "\\" + dir_files[i].name).c_str());

					else if (dir_files[i].file_type == "lua" || dir_files[i].file_type == "LUA")
						scr_editor->OpenScript(current_dir + "\\" + dir_files[i].name, dir_files[i].name);

					else if (dir_files[i].file_type == "rscn")
						mScene->LoadScene(p);

					else if (dir_files[i].file_type == "mat")
						mat_editor->EditMaterial(shortcurrent_dir + "\\" + dir_files[i].name);

					else if (dir_files[i].file_type == "rbp")
						mScene->SpawnEntity(p);

					else if (dir_files[i].is_mesh)
						mScene->SpawnModel(p);
					else
						ShellExecute(NULL, "open", (current_dir + "\\" + dir_files[i].name).c_str(), NULL, NULL, SW_SHOWDEFAULT);
				}

				ImGui::PopID();
				//ImGui::SameLine();
			}

			//ImGui::PushID(i + 2);
			//bool selectble = ImGui::Selectable(dir_files[i].name_only.c_str(), &dir_files[i].is_selected);
			//ImGui::PopID();
			if (!dir_files[i].is_dir)
			{
				// Display some info about that file
				std::string file_info = dir_files[i].name_only;
				if (dir_files[i].file_type == "rbp")
					file_info += " (Entity Prefab)";
				else if (dir_files[i].file_type == "mat")
					file_info += " (Material)";
				else if (dir_files[i].file_type == "bmp" || dir_files[i].file_type == "png"
					|| dir_files[i].file_type == "PNG" || dir_files[i].file_type == "BMP"
					|| dir_files[i].file_type == "JPG" || dir_files[i].file_type == "jpg")
					file_info += " (Texture)";
				else if (dir_files[i].file_type == "lua" || dir_files[i].file_type == "LUA")
					file_info += " (Lua Script)";
				else if (dir_files[i].file_type == "rscn")
					file_info += " (Scene)";
				else if (dir_files[i].file_type == "glb" || dir_files[i].file_type == "obj" || dir_files[i].file_type == "fbx" || dir_files[i].file_type == "FBX" || dir_files[i].file_type == "OBJ")
					file_info += " (Mesh)";
				else
					file_info += " (File)";

				DisplayInfo(file_info.c_str(), (shortcurrent_dir + "\\" + dir_files[i].name).c_str());
			}

			//ImGui::Text(dir_files[i].name_only.c_str(), ImVec2(55, 0));
			ImGui::NewLine();

			if (selectble)
			{
				if (dir_files[i].is_selected)
				{
					if (ImGui::GetIO().KeyCtrl) {
						dir_files[i].is_selected = true;
					}
					else
					{
						for (size_t j = 0; j < dir_files.size(); j++)
							dir_files[j].is_selected = false;
						dir_files[i].is_selected = true;
					}
				}
				else
				{
					if (ImGui::GetIO().KeyCtrl) {
						dir_files[i].is_selected = false;
					}
					else
					{
						for (size_t j = 0; j < dir_files.size(); j++)
							dir_files[j].is_selected = false;
						dir_files[i].is_selected = true;
					}
				}
			}
			ImGui::NextColumn();
		}
		foffset += f;
		ImGui::Columns(1);
	}
	/*for (size_t i = 0; i < dir_files.size(); i++)
	{
		bool selectble;
		if (icon_font != nullptr)
		{
			ImGui::PushFont(icon_font);
			std::string file_icon = "B"; // default
			if (dir_files[i].is_dir)
				file_icon = "A";
			else if(dir_files[i].file_type == "rbp")
				file_icon = "4";
			else if (dir_files[i].file_type == "lua" || dir_files[i].file_type == "LUA")
				file_icon = "y";
			else if (dir_files[i].file_type == "mat")
				file_icon ="m";
			else if (dir_files[i].file_type == "rscn")
				file_icon = "{";
			else if (dir_files[i].file_type == "obj" || dir_files[i].file_type == "fbx"
				|| dir_files[i].file_type == "OBJ" || dir_files[i].file_type == "FBX") {
				file_icon = "t";
				dir_files[i].is_mesh = true;
			}
			else if (dir_files[i].file_type == "bmp" || dir_files[i].file_type == "png"
				|| dir_files[i].file_type == "hdr" || dir_files[i].file_type == "PNG"
				|| dir_files[i].file_type == "BMP" || dir_files[i].file_type == "HDR"
				|| dir_files[i].file_type == "jpg" || dir_files[i].file_type == "JPG")
			{
				file_icon= "a";
				dir_files[i].is_texture = true;
			}

			 ImGui::PushID(i + 2);
			 selectble = ImGui::Selectable(file_icon.c_str(), &dir_files[i].is_selected, NULL, ImVec2(50, 0));
			 ImGui::PopFont();
			 if (!dir_files[i].is_dir && ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			 {
				 std::string drag_path = current_dir + "\\" + dir_files[i].name;
				 RGetRelativePath(drag_path);
				 if (dir_files[i].is_texture)
					 ImGui::SetDragDropPayload("RES_FILE_TEX", &drag_path, sizeof(std::string));
				 else if (dir_files[i].file_type == "mat")
					 ImGui::SetDragDropPayload("RES_FILE_MAT", drag_path.c_str(), drag_path.size());
				 else if (dir_files[i].file_type == "rbp")
					 ImGui::SetDragDropPayload("RES_FILE_RBP", &drag_path, sizeof(std::string));
				 else if (dir_files[i].file_type == "rscn")
					 ImGui::SetDragDropPayload("RES_FILE_SCENE", &drag_path, sizeof(std::string));
				 else if (dir_files[i].is_mesh)
					 ImGui::SetDragDropPayload("RES_FILE_MESH", &drag_path, sizeof(std::string));
				 ImGui::Text(drag_path.c_str());
				 ImGui::EndDragDropSource();
			 }

			 if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			 {
				 std::string p = current_dir + "\\" + dir_files[i].name;
				 RGetRelativePath(p);


				 // Enter directory
				 if (dir_files[i].is_dir)
					 OpenDir((current_dir + "\\" + dir_files[i].name).c_str());

				 else if (dir_files[i].file_type == "lua" || dir_files[i].file_type == "LUA")
					 scr_editor->OpenScript(current_dir + "\\" + dir_files[i].name, dir_files[i].name);

				 else if (dir_files[i].file_type == "rscn")
					 mScene->LoadScene(p);

				 else if (dir_files[i].file_type == "rbp")
					 mScene->SpawnEntity(p);

				 else if (dir_files[i].is_mesh)
					 mScene->SpawnModel(p);
				 else
					 ShellExecute(NULL, "open", (current_dir + "\\" + dir_files[i].name).c_str(), NULL, NULL, SW_SHOWDEFAULT);
			 }

			ImGui::PopID();
			//ImGui::SameLine();

		}
		ImGui::Button(dir_files[i].name_only.c_str(), ImVec2(50, 0));
		//ImGui::PushID(i + 2);
		//bool selectble = ImGui::Selectable(dir_files[i].name_only.c_str(), &dir_files[i].is_selected);
		//ImGui::PopID();
		if (!dir_files[i].is_dir)
		{
			// Display some info about that file
			std::string file_info = dir_files[i].name_only;
			if (dir_files[i].file_type == "rbp")
				file_info += " (Entity Prefab)\n";
			else if (dir_files[i].file_type == "mat")
				file_info += " (Material)\n";
			else if (dir_files[i].file_type == "bmp" || dir_files[i].file_type == "png"
				|| dir_files[i].file_type == "PNG" || dir_files[i].file_type == "BMP"
				|| dir_files[i].file_type == "JPG" || dir_files[i].file_type == "jpg")
				file_info += " (Texture)\n";
			else if (dir_files[i].file_type == "lua" || dir_files[i].file_type == "LUA")
				file_info += " (Lua Script)\n";
			else if (dir_files[i].file_type == "rscn")
				file_info += " (Scene)\n";
			else if (dir_files[i].file_type == "obj" || dir_files[i].file_type == "fbx" || dir_files[i].file_type == "FBX" || dir_files[i].file_type == "OBJ")
				file_info += " (Mesh)\n";
			else
				file_info += " (File)\n";
			file_info += shortcurrent_dir + "\\" + dir_files[i].name;

			DisplayInfo(file_info.c_str());
		}

		if(selectble)
		{
			if (dir_files[i].is_selected)
			{
				if (ImGui::GetIO().KeyCtrl) {
					dir_files[i].is_selected = true;
				}
				else
				{
					for (size_t j = 0; j < dir_files.size(); j++)
					{
						dir_files[j].is_selected = false;
					}
					dir_files[i].is_selected = true;
				}
			}
			else
			{
				if (ImGui::GetIO().KeyCtrl) {
					dir_files[i].is_selected = false;
				}
				else
				{
					for (size_t j = 0; j < dir_files.size(); j++)
					{
						dir_files[j].is_selected = false;
					}
					dir_files[i].is_selected = true;
				}
			}
		}

		//ImGui::Separator();
	}*/
	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1))
	{
		ImGui::OpenPopup("resEDIT");
	}
	if (ImGui::BeginPopup("resEDIT"))
	{
		ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
		if (ImGui::BeginMenu("Create Folder"))
		{
			ImGui::Text("Folder Name ");
			ImGui::SameLine();
			ImGui::InputText("##FNAME", &folder_name);
			if (ImGui::Selectable("Create"))
			{
				CreateDir(folder_name);
				OpenDir(current_dir.c_str());
				folder_name = "New Folder";
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Create New Script"))
		{
			ImGui::Text("Script Name ");
			ImGui::SameLine();
			ImGui::InputText("##SNAME", &scr_name);
			if (ImGui::Selectable("Create"))
			{
				if (!CheckForFile(scr_name + ".lua"))
				{
					std::ofstream outfile(current_dir + "\\" + scr_name + ".lua");

					std::string script_new = default_Script;
					StrReplaceAll(script_new, "XXX", scr_name);
					outfile << script_new << std::endl;

					outfile.close();

					OpenDir(current_dir.c_str());
				}
				scr_name = "NewScript";
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Create New Material"))
		{
			ImGui::Text("Material Name ");
			ImGui::SameLine();
			ImGui::InputText("##MAT_NAME", &mat_name);
			if (ImGui::Selectable("Create"))
			{
				if (!CheckForFile(mat_name + ".mat"))
				{
					std::string mt_name = current_dir + "\\" + mat_name + ".mat";
					RGetRelativePath(mt_name);
					rnder->CreateMaterialOnDisk(mt_name.c_str());
					OpenDir(current_dir.c_str());
				}
				mat_name = "NewMaterial";
			}

			ImGui::EndMenu();
		}
		int sel_idx = GetSelectionIndex();
		if (sel_idx != -1 && dir_files[sel_idx].is_texture && (ImGui::Selectable("Create Material From Texture")))
		{
			if (!CheckForFile(dir_files[sel_idx].name_only + ".mat"))
			{
				std::string mt_name = current_dir + "\\" + dir_files[sel_idx].name_only + ".mat";
				std::string tex_path = current_dir + "\\" + dir_files[sel_idx].name;
				RGetRelativePath(mt_name);
				RGetRelativePath(tex_path);
				rnder->CreateMaterialOnDiskWithTexture(mt_name, tex_path);
				OpenDir(current_dir.c_str());
			}
		}
		ImGui::Separator();
		if (ImGui::Selectable("Refresh"))
		{
			OpenDir(current_dir.c_str());
		}
		ImGui::Separator();
		if (ImGui::Selectable("Copy"))
		{
			last_operation_is_cut = false;
			last_copied.clear();
			for (size_t i = 0; i < dir_files.size(); i++)
			{
				if (dir_files[i].is_selected && !dir_files[i].is_dir)
				{
					last_copied.push_back(current_dir + "\\" + dir_files[i].name);
				}
			}
		}
		if (ImGui::Selectable("Cut"))
		{
			last_operation_is_cut = true;
			last_copied.clear();
			for (size_t i = 0; i < dir_files.size(); i++)
			{
				if (dir_files[i].is_selected && !dir_files[i].is_dir)
				{
					last_copied.push_back(current_dir + "\\" + dir_files[i].name);
				}
			}
		}
		if (ImGui::Selectable("Past"))
		{
			for (size_t i = 0; i < last_copied.size(); i++)
			{
				std::ifstream  src(last_copied[i], std::ios::binary);
				if (src.good()) {

					int slashPos = 0;
					for (size_t j = last_copied[i].length() - 1; j > 0; j--)
					{
						if (last_copied[i][j] == '\\')
						{
							slashPos = j;
							break;
						}
					}
					std::string name_only = last_copied[i].substr(slashPos + 1, std::string::npos);
					int point_pos = 0;
					for (size_t j = name_only.length() - 1; j > 0; j--)
					{
						if (name_only[j] == '.')
						{
							point_pos = j;
							break;
						}
					}
					std::string new_dir;
					if (last_operation_is_cut)
						new_dir = current_dir + "\\" + name_only;
					else
						new_dir = current_dir + "\\" + name_only.substr(0, point_pos) + "(Copy)" + name_only.substr(point_pos, std::string::npos);
					std::ofstream  dst(new_dir, std::ios::binary);
					dst << src.rdbuf();
					src.close();
					dst.close();
					// if cuting delete cuted files.
					if (last_operation_is_cut)
					{
						std::remove(last_copied[i].c_str());
					}
				}
			}
			last_copied.clear();
			OpenDir(current_dir.c_str());
		}
		if (ImGui::Selectable("Deplicate"))
		{
			for (size_t i = 0; i < dir_files.size(); i++)
			{
				if (dir_files[i].is_selected && !dir_files[i].is_dir)
				{
					std::ifstream  src(current_dir + "\\" + dir_files[i].name, std::ios::binary);
					if (src.good()) {
						std::ofstream  dst(current_dir + "\\" + dir_files[i].name_only + "(Copy)." + dir_files[i].file_type, std::ios::binary);
						dst << src.rdbuf();
						src.close();
						dst.close();
					}
				}
			}
			OpenDir(current_dir.c_str());
		}
		if (ImGui::Selectable("Delete"))
		{
			for (size_t i = 0; i < dir_files.size(); i++)
			{
				if (dir_files[i].is_selected)
				{
					if (dir_files[i].is_dir)
						std::experimental::filesystem::remove_all(current_dir + "\\" + dir_files[i].name);
					else
						std::remove((current_dir + "\\" + dir_files[i].name).c_str());
				}
			}
			OpenDir(current_dir.c_str());
		}
		ImGui::Separator();
		if (ImGui::Selectable("Show in explorer"))
		{
			//system(("explorer " + current_dir).c_str()); // not portable
			ShellExecute(NULL, "open", current_dir.c_str(), NULL, NULL, SW_SHOWDEFAULT);
		}
		ImGui::PopStyleColor();
		ImGui::EndPopup();
	}
	ImGui::PopStyleColor();
	ImGui::EndChild();
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
		{
			IM_ASSERT(payload->DataSize == sizeof(EnttID));
			EnttID payload_n = *(const int*)payload->Data;
			Entity* target = mScene->FindEntity(payload_n);
			if (target != nullptr)
				target->SaveEntityFile((shortcurrent_dir + "\\" + target->name + ".rbp").c_str());
			RefreshDir();
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::End();

	//ImGui::ImageButton

	if (ImGui::BeginPopupModal("Create Folder ##resfoldercretor"))
	{
		ImGui::Text("Folder Name ");
		ImGui::SameLine();
		ImGui::InputText("#FNAME", &folder_name);
		if (ImGui::Button("Create", ImVec2(ImGui::GetWindowWidth(), 0)))
		{
			CreateDir(folder_name);
			folder_name = "New Folder";
		}
		ImGui::EndPopup();
	}
}
// --------------------------------------------------------
bool EProjectBrowser::ImageSelectableV(const char * label, ImTextureID img, bool selected, ImGuiSelectableFlags flags, const ImVec2 & size_arg, ImVec4 colr, const char* txt)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	if ((flags & ImGuiSelectableFlags_SpanAllColumns) && window->DC.CurrentColumns) // FIXME-OPT: Avoid if vertically clipped.
		ImGui::PushColumnsBackground();

	ImGuiID id = window->GetID(txt);
	ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	ImVec2 size(size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y);
	ImVec2 pos = window->DC.CursorPos;
	pos.y += window->DC.CurrLineTextBaseOffset;
	ImRect bb_inner(pos, addv2(pos , size));
	ImGui::ItemSize(size);

	// Fill horizontal space.
	ImVec2 window_padding = window->WindowPadding;
	float max_x = (flags & ImGuiSelectableFlags_SpanAllColumns) ? ImGui::GetWindowContentRegionMax().x : ImGui::GetContentRegionMax().x;
	float w_draw = ImMax(label_size.x, window->Pos.x + max_x - window_padding.x - pos.x);
	ImVec2 size_draw((size_arg.x != 0 && !(flags & ImGuiSelectableFlags_DrawFillAvailWidth)) ? size_arg.x : w_draw, size_arg.y != 0.0f ? size_arg.y : size.y);
	ImRect bb(pos, addv2(pos , size_draw));
	if (size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_DrawFillAvailWidth))
		bb.Max.x += window_padding.x;

	// Selectables are tightly packed together so we extend the box to cover spacing between selectable.
	const float spacing_x = style.ItemSpacing.x;
	const float spacing_y = style.ItemSpacing.y;
	const float spacing_L = (float)(int)(spacing_x * 0.50f);
	const float spacing_U = (float)(int)(spacing_y * 0.50f);
	bb.Min.x -= spacing_L;
	bb.Min.y -= spacing_U;
	bb.Max.x += (spacing_x - spacing_L);
	bb.Max.y += (spacing_y - spacing_U);

	bool item_add;
	if (flags & ImGuiSelectableFlags_Disabled)
	{
		ImGuiItemFlags backup_item_flags = window->DC.ItemFlags;
		window->DC.ItemFlags |= ImGuiItemFlags_Disabled | ImGuiItemFlags_NoNavDefaultFocus;
		item_add = ImGui::ItemAdd(bb, id);
		window->DC.ItemFlags = backup_item_flags;
	}
	else
	{
		item_add = ImGui::ItemAdd(bb, id);
	}
	if (!item_add)
	{
		if ((flags & ImGuiSelectableFlags_SpanAllColumns) && window->DC.CurrentColumns)
			ImGui::PopColumnsBackground();
		return false;
	}

	// We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
	ImGuiButtonFlags button_flags = 0;
	if (flags & ImGuiSelectableFlags_NoHoldingActiveID) button_flags |= ImGuiButtonFlags_NoHoldingActiveID;
	if (flags & ImGuiSelectableFlags_PressedOnClick) button_flags |= ImGuiButtonFlags_PressedOnClick;
	if (flags & ImGuiSelectableFlags_PressedOnRelease) button_flags |= ImGuiButtonFlags_PressedOnRelease;
	if (flags & ImGuiSelectableFlags_Disabled) button_flags |= ImGuiButtonFlags_Disabled;
	if (flags & ImGuiSelectableFlags_AllowDoubleClick) button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick;
	if (flags & ImGuiSelectableFlags_AllowItemOverlap) button_flags |= ImGuiButtonFlags_AllowItemOverlap;

	if (flags & ImGuiSelectableFlags_Disabled)
		selected = false;

	const bool was_selected = selected;
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, button_flags);

	// Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
	if (pressed || (hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
	{
		if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
		{
			g.NavDisableHighlight = true;
			ImGui::SetNavID(id, window->DC.NavLayerCurrent);
		}
	}
	if (pressed)
		ImGui::MarkItemEdited(id);

	if (flags & ImGuiSelectableFlags_AllowItemOverlap)
		ImGui::SetItemAllowOverlap();

	// In this branch, Selectable() cannot toggle the selection so this will never trigger.
	if (selected != was_selected) //-V547
		window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

	// Render
	if (held && (flags & ImGuiSelectableFlags_DrawHoveredWhenHeld))
		hovered = true;
	
	if (hovered)
		ImGui::RenderFrame(addv2(bb.Min, ImVec2(-1.0f, -1.0f)), addv2(bb.Max, ImVec2(1.0f, 1.0f)), ImGui::GetColorU32(ImVec4(0.99f, 0.67f, 0.054f, 1.0f)), false, 4.0f);
	else
		ImGui::RenderFrame(bb.Min, addv2(bb.Max, ImVec2(2.1f, 2.1f)), ImGui::GetColorU32(ImVec4(0.06f, 0.06f, 0.06f, 1.0f)), false, 4.0f);

	if (selected)
	{
		//const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
		const ImU32 col = ImGui::GetColorU32(ImGuiCol_Header);
		ImGui::RenderFrame(bb.Min, bb.Max, col, false, 4.0f);
		ImGui::RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);
	}
	else 
	{
		ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImVec4(0.088f, 0.088f, 0.088f, 1.0f)), false, 4.0f);
	}
	/*if (hovered || selected)
	{
		const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
		ImGui::RenderFrame(bb.Min, bb.Max, col, false, 4.0f);
		ImGui::RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);
	}
	else
		ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImVec4(0.088f, 0.088f, 0.088f, 1.0f)), false, 4.0f);*/

	if ((flags & ImGuiSelectableFlags_SpanAllColumns) && window->DC.CurrentColumns)
	{
		ImGui::PopColumnsBackground();
		bb.Max.x -= (ImGui::GetContentRegionMax().x - max_x);
	}

	window->DrawList->AddImage(img, bb.Min, addv2(bb.Max, ImVec2(0.0f, -37.0f)), ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(colr));
	//ImGui::RenderFrame(addv2(bb.Min, ImVec2(0.0f, bb.Max.y + 1.0f)), addv2(bb.Max, ImVec2(0.0f, 2.0f)), ImGui::GetColorU32(ImVec4(0.80f, 0.67f, 0.054f, 1.0f)), false, 4.0f);

	if (flags & ImGuiSelectableFlags_Disabled) ImGui::PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_Text]);
	ImGui::RenderTextClipped(addv2(bb_inner.Min, ImVec2(0.0f, 90)), bb_inner.Max, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	if (flags & ImGuiSelectableFlags_Disabled) ImGui::PopStyleColor();

	// Automatically close popups
	if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiSelectableFlags_DontClosePopups) && !(window->DC.ItemFlags & ImGuiItemFlags_SelectableDontClosePopup))
		ImGui::CloseCurrentPopup();

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
	return pressed;
}
// --------------------------------------------------------
bool EProjectBrowser::ImageSelectable(const char * label, ImTextureID img, bool * p_selected, ImGuiSelectableFlags flags, const ImVec2 & size_arg, ImVec4 colr, const char* txt)
{
	if (ImageSelectableV(label, img, *p_selected, flags, size_arg, colr, txt))
	{
		*p_selected = !*p_selected;
		return true;
	}
	return false;
}
// --------------------------------------------------------
ImVec2 EProjectBrowser::addv2(ImVec2 & a, ImVec2 & b)
{
	return ImVec2(a.x + b.x, a.y + b.y);
}
// --------------------------------------------------------
void EProjectBrowser::GoBack()
{
	// Don't go outside the resources folder.
	if (current_dir == res_dir)
		return;

	int slashPos = 0;
	for (size_t i = current_dir.length() - 1; i > 0; i--)
	{
		if (current_dir[i] == '\\')
		{
			slashPos = i;
			break;
		}
	}
	OpenDir(current_dir.substr(0, slashPos).c_str());
}
std::string & EProjectBrowser::GetFileDir(unsigned int fileIndx)
{
	return shortcurrent_dir + "\\" + dir_files[fileIndx].name;
}
// --------------------------------------------------------
void EProjectBrowser::OpenDir(const char* path)
{
	struct dirent **files;

	current_dir = path;
	shortcurrent_dir = current_dir;
	RGetRelativePath(shortcurrent_dir);

	if (!dir_files.empty())
		dir_files.clear();

	int n = scandir(path, &files, nullptr, alphasort);
	if (n > 0)
	{
		for (int i = 0; i < n; i++)
		{
			if (files[i]->d_type != DT_DIR || files[i]->d_name == "." || files[i]->d_name == "..")
				continue;

			std::string fname = files[i]->d_name;

			if (fname == "." || fname == "..")
				continue;

			dir_files.push_back(EFileInfo(fname, fname, "", true));
		}

		for (int i = 0; i < n; i++)
		{
			if (files[i]->d_type != DT_REG)
			{
				free(files[i]);
				continue;
			}

			std::string fname = files[i]->d_name;
			int point_pos = 0;
			bool has_type = false;
			for (size_t i = fname.length() - 1; i > 0; i--)
			{
				if (fname[i] == '.')
				{
					point_pos = i;
					has_type = true;
					break;
				}
			}
			std::string ftype = fname;
			ftype = fname.substr(point_pos + 1, std::string::npos);

			if (has_type) 
			{
				dir_files.push_back(EFileInfo(fname, fname.substr(0, point_pos), ftype, false));

				if (ftype == "mat")
				{
					thumbnails.emplace_back(FileThumbnail::FT_Material, (shortcurrent_dir + "\\" + fname).c_str());
					std::cout << "thumbnail : " << (shortcurrent_dir + "\\" + fname).c_str() << "\n";
				}
			}

			free(files[i]);
		}
		free(files);
	}
}
// --------------------------------------------------------
bool EProjectBrowser::IsDirectoryExist(const std::string& name)
{
	bool bExists = false;

	if (!name.empty())
	{
		DIR *pDir = nullptr;
		pDir = opendir(name.c_str());
		if (pDir != nullptr)
		{
			bExists = true;
			(void)closedir(pDir);
		}
	}

	return bExists;
}
// --------------------------------------------------------
bool EProjectBrowser::CreateDirectoryIfNotExist(const std::string& name)
{
	bool res = false;

	if (!name.empty())
	{
		if (!IsDirectoryExist(name))
		{
			res = true;

#ifdef WIN32
			CreateDirectoryA(name.c_str(), nullptr);
#elif defined(LINUX) or defined(APPLE)
			char buffer[PATH_MAX] = {};
			snprintf(buffer, PATH_MAX, "mkdir -p %s", name.c_str());
			const int dir_err = std::system(buffer);
			if (dir_err == -1)
			{
				std::cout << "Error creating directory " << name << std::endl;
				res = false;
			}
#endif
		}
	}

	return res;
}
// --------------------------------------------------------
bool EProjectBrowser::CreateDir(const std::string& vPath)
{
	bool res = false;

	if (!vPath.empty())
	{
		std::string path = current_dir + "\\" + vPath;

		res = CreateDirectoryIfNotExist(path);
	}

	return res;
}
// --------------------------------------------------------
bool EProjectBrowser::CheckForFile(std::string & fname)
{
	for (size_t i = 0; i < dir_files.size(); i++)
	{
		if (dir_files[i].name == fname)
			return true;
	}
	return false;
}
// --------------------------------------------------------