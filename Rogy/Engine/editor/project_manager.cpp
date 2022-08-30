#include "project_manager.h"

#include <fstream>
#include <sstream>

ProjectManager::ProjectManager()
{
}

ProjectManager::~ProjectManager()
{
}
// --------------------------------------------------------
void ProjectManager::Init(std::string engine_directory)
{
	engine_dir = engine_directory;
	ScanDirForProjects(engine_dir);
	ProjectName = "New Project";
}
// --------------------------------------------------------
void ProjectManager::Render()
{
	if (!isOn)
		return;

	ImGui::Begin("Projects", &isOn, ImGuiWindowFlags_None);

	ImGui::BeginChild("ctrl_buttons", ImVec2(ImGui::GetWindowWidth() / 4.0f, 0.0f));
	bool is_in_projects = !createNew;
	ImGui::Separator();
	if (ImGui::Selectable("Projects", &is_in_projects, 0, ImVec2(0.0f, 50.0f)))
	{
		createNew = false;
	}
	ImGui::Separator();
	if(ImGui::Selectable("New", &createNew, 0, ImVec2(0.0f, 50.0f)))
	{
		createNew = true;
	}
	ImGui::Separator();
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("projects_tab");
	if (createNew)
	{
		ImGui::Text("New Project");
		ImGui::Separator();

		ImGui::Text("Create a new empty Rogy Project.");
		ImGui::NewLine();

		ImGui::Text("Project Name");
		ImGui::SameLine();
		ImGui::InputTextWithHint("##_prjnamenew", "Project Name", &ProjectName);
		ImGui::NewLine();
		float width = ImGui::GetWindowWidth() - 300;
		ImGui::SetCursorPosX(width);
		if (ImGui::Button(" Create "))
		{
			CreateNewProject();
		}
	}
	else
	{
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth());
		ImGui::InputTextWithHint("##_prjfilter", "Search...", &scrfilter);
		ImGui::Separator();

		for (size_t i = 0; i < ProjectsList.size(); i++)
		{
			bool startswith = true;
			if (!scrfilter.empty())
			{
				for (size_t j = 0; j < scrfilter.size(); j++)
				{
					if (scrfilter[j] != ProjectsList[i][j])
					{
						startswith = false;
						break;
					}
				}
			}
			if (startswith)
			{
				ImGui::Image(logoID, ImVec2(50.0f, 50.0f));
				ImGui::SameLine();
				if (ImGui::Selectable(ProjectsList[i].c_str(), &isOn, ImGuiTreeNodeFlags_SpanFullWidth, ImVec2(0.0f, 50.0f)))
				{
					OpenProject(ProjectsList[i].c_str());
				}
				ImGui::Separator();
			}
		}
		
	}
	ImGui::EndChild();
	ImGui::End();
}
// --------------------------------------------------------
bool ProjectManager::CreateNewProject()
{
	isOn = false;
	std::cout << "Creating Project : " << ProjectName << std::endl;
	bool sucess = CreateDirectoryIfNotExist(ProjectName);

	if (sucess)
	{
		OpenProject(ProjectName);
	}
	else
	{
		std::cout << "Unable to create project  : Folder already exists!" << std::endl;
	}

	ProjectName = "New Project";
	return sucess;
}
// --------------------------------------------------------

void ProjectManager::startupProject()
{
	// additional information
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	
	// start the program up
	CreateProcess("Rogy.exe",   // the path
		"",        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	);
	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

void ProjectManager::OpenProject(std::string dir)
{
	isOn = false;
	std::cout << "Openning Project : " << dir << std::endl;
	// ---------------------------------------
	// Save next startup project.
	std::ofstream fout("core\\startup");
	fout << dir;
	fout.close();
	// ---------------------------------------
	startupProject();
}
// --------------------------------------------------------
void ProjectManager::ScanDirForProjects(std::string dir)
{
	struct dirent** files;

	int n = scandir(dir.c_str(), &files, nullptr, alphasort);
	if (n > 0)
	{
		for (int i = 0; i < n; i++)
		{
			if (files[i]->d_type != DT_DIR || files[i]->d_name == "." || files[i]->d_name == "..")
				continue;

			std::string fname = files[i]->d_name;

			if (fname == "." || fname == "..")
				continue;

			std::string newDir = dir;
			newDir += "\\";
			newDir += fname;

			struct dirent** in_files;
			int n2 = scandir(newDir.c_str(), &in_files, nullptr, alphasort);
			if (n2 == 0)
			{
				free(in_files);
				free(files[i]);
				continue;
			}

			for (int j = 0; j < n2; j++)
			{
				std::string pname = in_files[j]->d_name;
				if (pname == "ProjectSettings")
				{
					// PROJECT DETECTED
					//std::cout << "Detected Project -> " << fname << std::endl;
					ProjectsList.emplace_back(fname);
				}

				free(in_files[j]);
			}
			free(in_files);

			free(files[i]);
		}
		free(files);
	}
}

// --------------------------------------------------------
bool ProjectManager::IsDirectoryExist(const std::string& name)
{
	bool bExists = false;

	if (!name.empty())
	{
		DIR* pDir = nullptr;
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
bool ProjectManager::CreateDirectoryIfNotExist(const std::string& name)
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