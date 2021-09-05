#include "project_browser.h"
#include <experimental\filesystem>

// --------------------------------------------------------
EProjectBrowser::EProjectBrowser()
{
}
// --------------------------------------------------------
EProjectBrowser::~EProjectBrowser()
{
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
// --------------------------------------------------------
void EProjectBrowser::Render(ImFont* icon_font)
{
	if (!initialized)
	{
		initialized = true;
		current_dir = RGetCurrentPath();
		res_dir = current_dir + "\\res";
		folder_name = "New Folder";
		scr_name = "NewScript";
		mat_name = "NewMaterial";
		default_Script = "XXX = ScriptComponent()  \n\n"
			"-- class constructor, define instance variables\n"
			"function XXX: init()\n"
			"\t--self.Var = 1.0;\n"
			"\t--self._localVar = 1.0;\n"
			"end\n\n"
			"-- OnStart called when the entity starts\n"
			"function XXX: OnStart()\n\n"
			"end\n\n"
			"-- OnUpdate called every frame.\n"
			"function XXX: OnUpdate(dt)\n\n"
			"end\n";

		OpenDir(res_dir.c_str());
	}

	if (!isOn) return;

	ImGui::Begin("Project Browser", &isOn, ImGuiWindowFlags_MenuBar);
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
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::MenuItem("<"))
			GoBack();

		ImGui::Text(shortcurrent_dir.c_str());
		ImGui::EndMenuBar();
	}
	ImGui::BeginChild("pb_Child");
	
	//ImGui::Separator();
	if (dir_files.empty())
	{
		ImGui::Text(""); ImGui::Text("");
		ImGui::SameLine(0, ImGui::GetWindowWidth() / 2 - 60);
		ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 0.6f), "This folder is empty");
	}
	size_t f = ImGui::GetWindowWidth() / 70;
	if (f < 1) f = 1;
	size_t lineCount = (dir_files.size() / f) + 1;
	if (lineCount < 1) f = 1;
	size_t foffset = 0;
	for (size_t j = 0; j < lineCount; j++)
	{
		ImGui::Columns(f, NULL, false);
		for (size_t i = foffset; i < foffset + f && i < dir_files.size(); i++)
		{
			bool selectble;
			if (icon_font != nullptr)
			{
				ImGui::PushFont(icon_font);
				std::string file_icon = "B"; // default
				if (dir_files[i].is_dir)
					file_icon = "A";
				else if (dir_files[i].file_type == "rbp") {
					file_icon = "4";
					dir_files[i].is_prefab = true;
				}
				else if (dir_files[i].file_type == "lua" || dir_files[i].file_type == "LUA")
					file_icon = "y";
				else if (dir_files[i].file_type == "mat")
					file_icon = "m";
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
					file_icon = "a";
					dir_files[i].is_texture = true;
				}

				ImGui::PushID(i + 2);
				selectble = ImGui::Selectable(file_icon.c_str(), &dir_files[i].is_selected, NULL, ImVec2(55, 50));
				ImGui::PopFont();
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
				else if (dir_files[i].file_type == "obj" || dir_files[i].file_type == "fbx" || dir_files[i].file_type == "FBX" || dir_files[i].file_type == "OBJ")
					file_info += " (Mesh)";
				else
					file_info += " (File)";

				DisplayInfo(file_info.c_str(), (shortcurrent_dir + "\\" + dir_files[i].name).c_str());
			}

			ImGui::Text(dir_files[i].name_only.c_str(), ImVec2(55, 0));

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
		ImGui::EndPopup();
	}
	ImGui::EndChild();
	ImGui::End();



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
			for (size_t i = fname.length() - 1; i > 0; i--)
			{
				if (fname[i] == '.')
				{
					point_pos = i;
					break;
				}
			}
			dir_files.push_back(EFileInfo(fname, fname.substr(0, point_pos), fname.substr(point_pos + 1, std::string::npos), false));

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