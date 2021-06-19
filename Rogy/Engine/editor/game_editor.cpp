#include "game_editor.h"

ImFont* tex;
ImFont* Smalltex;
ImFont* codeTex;

Game_Editor::Game_Editor()
{
}

Game_Editor::~Game_Editor()
{
}

bool Game_Editor::Init(char* glsl_version, GLFWwindow* window, bool platform_support)
{
	std::cout << "\nInitializing Editor ..." << std::endl;
	platformSupport = platform_support;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	if (platform_support) 
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();

	style.WindowRounding = 0.0f;
	style.TabRounding = 0.0f;
	style.WindowBorderSize = 0;
	style.ScrollbarSize = 10;
	style.ScrollbarRounding = 13;
	//style.FrameRounding = 3;

	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);

	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.28f, 0.28f, 1.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.32f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);

	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);


	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0, 0, 0, 0.4f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.f, 0.f, 0.f, 0.f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.f, 0.f, 0.f, 0.f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.f, 0.f, 0.f, 0.f);

	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.23f, 0.23f, 0.23f, 1.f);

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Load Fonts 
	io.Fonts->AddFontFromFileTTF("core/font/DroidSans.ttf", 30);
	tex = io.Fonts->AddFontFromFileTTF("core/font/Icons.ttf", 35);
	Smalltex = io.Fonts->AddFontFromFileTTF("core/font/Icons.ttf", 30);
	codeTex = io.Fonts->AddFontFromFileTTF("core/font/consola.ttf", 33);
	
	s_hierarchy.icon_small = Smalltex;
	prep_editor.nodes = &s_hierarchy;
	prj_browser.scr_editor = &code_editor;

	viewport = ImGui::GetMainViewport();
	io.FontGlobalScale = 0.5f;
	return true;
}

bool Game_Editor::isMouseInEditor()
{
	ImVec2 mousePos = ImGui::GetIO().MousePos;
	if (mousePos.x == -FLT_MAX && mousePos.y == -FLT_MAX)
		return false;
	return true;
}

bool Game_Editor::BeginWindow(char* name)
{
	bool b = ImGui::Begin(name, NULL, ImGuiWindowFlags_NoTitleBar);
	ImGui::Text(name);
	ImGui::SameLine(0, ImGui::GetWindowWidth() - 55);

	ImGui::PushFont(tex);
	ImGui::Button(";");
	ImGui::PopFont();

	ImGui::BeginChild(string(name + 1).c_str(), ImVec2(0,0), false, ImGuiWindowFlags_NoTitleBar || ImGuiWindowFlags_MenuBar);

	return b;
}

void Game_Editor::EndWindow()
{
	ImGui::EndChild();
	ImGui::End();
}
void Game_Editor::start()
{
	// Start the Dear ImGui frame
	//viewport = ImGui::GetMainViewport();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	DockSpaces();
	s_hierarchy.Render();
	scn_settings.Render();
	prep_editor.Render();
	db_editor.Render();
	code_editor.Render(codeTex);
	prj_browser.Render(tex);
	prg_settings.Render();

	/*ImGui::SetNextWindowPos(ImVec2(200, 200));
	ImGui::SetNextWindowBgAlpha(0.0f);
	ImGui::Begin("MyButton", NULL, ImGuiWindowFlags_NoBackground || ImGuiWindowFlags_NoCollapse ||
									ImGuiWindowFlags_NoDocking || ImGuiWindowFlags_NoMove ||
									ImGuiWindowFlags_NoResize || ImGuiWindowFlags_NoTitleBar
									|| ImGuiWindowFlags_NoSavedSettings || ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::Button("Press this Button"))
		db_editor.debuger->Log("Button Pressed");
	ImGui::End();*/
}
void Game_Editor::render()
{
	ToolBar();
	DownBar();

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void Game_Editor::ShutDown()
{
	cout << "editor shutdown ...\n";
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	//ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Game_Editor::BeginPreps(char* nm)
{
	ImGui::Columns(2, nm, false);
	ImGui::SetColumnWidth(0, ImGui::GetWindowWidth()*0.4f);
}

void Game_Editor::PrepName(char* p_name)
{
	ImGui::Dummy(ImVec2(0, 0.17f));
	ImGui::Text(p_name);
}

void Game_Editor::NextPreps()
{
	ImGui::NextColumn();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 2);
}

void Game_Editor::EndPreps()
{
	//ImGui::EndColumns();
}

void Game_Editor::EditVec3_xyz(const char* name_id, glm::vec3 &values)
{
	float siz = (ImGui::GetWindowWidth()/2)/3;
	string col_s("##1");
	col_s += name_id;
	ImGui::SetNextItemWidth(siz);
	ImGui::DragFloat(col_s.c_str(), &values.x, 0.1f, 0, 0, "X: %.3g"); ImGui::SameLine();
	ImGui::SetNextItemWidth(siz);
	col_s += "2";
	ImGui::DragFloat(col_s.c_str(), &values.y, 0.1f, 0, 0, "Y: %.3g"); ImGui::SameLine();
	ImGui::SetNextItemWidth(siz);
	col_s += "3";
	ImGui::DragFloat(col_s.c_str(), &values.z, 0.1f, 0, 0, "Z: %.3g");
	col_s.clear();
}

void Game_Editor::CreateEntityWithMesh(std::string ent_name, std::string mesh_path, const char* col_type)
{
	Entity* e = s_hierarchy.scene->AddEntity(-1, ent_name);
	RendererComponent* rc = prep_editor.rndr->m_renderers.AddComponent(e->ID);
	rc->mesh = prep_editor.res->mMeshs.CreateModel(mesh_path)->GetFirstMesh();
	rc->material = prep_editor.rndr->CreateMaterial("");
	e->AddComponent<RendererComponent>(rc);

	if (col_type == "Box")
	{
		prep_editor.phy_world->AddRigidBody(e, 1.0f);
		// STATIC
		e->Static = true;
		if (e->HasComponent<RigidBody>())
			e->GetComponent<RigidBody>()->SetBodyMode(RB_STATIC);
	}
	else if (col_type == "Sphere")
	{
		prep_editor.phy_world->AddRigidBody(e);
		e->GetComponent<RigidBody>()->ChangeShape(RCollisionShapeType::SPHERE_COLLIDER);
	}
	else if (col_type == "Capsule")
	{
		prep_editor.phy_world->AddRigidBody(e);
		e->GetComponent<RigidBody>()->ChangeShape(RCollisionShapeType::CAPSULE_COLLIDER);
	}
	else if (col_type == "Mesh" && rc != nullptr && rc->mesh != nullptr)
	{
		prep_editor.phy_world->AddRigidBody(e);
		e->GetComponent<RigidBody>()->SetCollisionMesh(prep_editor.phy_world->GetMeshCollider(rc->mesh), mesh_path);
	}
}

void Game_Editor::ToolBar()
{
	if (!s_hierarchy.sel_entt.empty() && s_hierarchy.input->GetKey(RKey::KEY_LCTRL))
	{
		if (s_hierarchy.input->GetKeyDown(RKey::KEY_A))
		{
			if (s_hierarchy.GetSelected() != -1)
			{
				Entity* ent = s_hierarchy.scene->FindEntity(s_hierarchy.GetSelected());
				if (ent != nullptr && ent->HasComponent<RendererComponent>())
				{
					// Get Selected files
					for (size_t i = 0; i < prj_browser.dir_files.size(); i++)
					{
						if (prj_browser.dir_files[i].is_selected)
						{
							// If it is a material use it in the current mesh
							if (prj_browser.dir_files[i].file_type == "mat")
							{
								std::string asset_path = prj_browser.current_dir + "\\";
								asset_path += prj_browser.dir_files[i].name;
								RGetRelativePath(asset_path);
								Material* mat = prep_editor.rndr->LoadMaterial(asset_path.c_str());
								mat->path = prj_browser.dir_files[i].name;
								if (mat != nullptr)
									ent->GetComponent<RendererComponent>()->material = mat;
							}
							break;
						}
					}
				}
			}
		}
	}

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows,
			// which we can't undo at the moment without finer window depth/z control.
			//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
			if (ImGui::MenuItem("New Scene", "Ctrl+N"))
			{
				s_hierarchy.scene->NewScene();
			}
			if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
			{
				ImGuiFileDialog::Instance()->OpenModal("LoadScnModel", "Open Scene", ".rscn\0", ".");
			}
			if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
			{
				if (s_hierarchy.scene->path == "")
					ImGuiFileDialog::Instance()->OpenModal("SaveScnModel", "Save Scene", ".rscn\0", ".");
				else
					s_hierarchy.scene->SaveScene(s_hierarchy.scene->path);
			}
			if (ImGui::MenuItem("Save Scene As"))
			{
				ImGuiFileDialog::Instance()->OpenModal("SaveScnModel", "Save Scene", ".rscn\0", ".");
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit")) {
				s_hierarchy.scene->QuitGame();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "Ctrl+Z"))
			{
			}
			if (ImGui::MenuItem("Redo", "Ctrl+Shift+Z"))
			{
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Play", "Ctrl+P"))
			{
			}
			if (ImGui::MenuItem("Pause", "Ctrl+Shift+S"))
			{
			}
			if (ImGui::MenuItem("Stop", "Ctrl+Shift+P"))
			{
			}
			ImGui::Separator();

			if (ImGui::MenuItem("Project Settings"))
			{
			}
			if (ImGui::MenuItem("Grid"))
			{
				s_hierarchy.scene->show_grid = !s_hierarchy.scene->show_grid;
			}

			ImGui::Separator();
			if (!s_hierarchy.Empty() && ImGui::MenuItem("Create Entity prefab"))
			{
				Entity* ent = s_hierarchy.scene->FindEntity(s_hierarchy.GetSelected());
				if (ent != nullptr) 
				{
					std::string saveP = prj_browser.current_dir + "\\" + ent->name + ".rbp";
					RGetRelativePath(saveP);
					s_hierarchy.scene->SaveEntityPrefab(ent->ID, saveP.c_str());
				}
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Add"))
		{
			if (ImGui::Selectable("New Entity"))
			{
				s_hierarchy.scene->AddEntity(-1, "New Entity");
			}
			if (ImGui::Selectable("Load Entity")) // load entity
				ImGuiFileDialog::Instance()->OpenModal("ChooseBPModel", "Select Prefab", ".rbp\0", ".");

			ImGui::Separator();
			if (ImGui::Selectable("Cube"))
			{
				CreateEntityWithMesh("Cube", "core\\models\\cube.fbx", "Box");
			}
			if (ImGui::Selectable("Capsule"))
			{
				CreateEntityWithMesh("Capsule", "core\\models\\capsule.fbx", "Capsule");
			}
			if (ImGui::Selectable("Plane"))
			{
				CreateEntityWithMesh("Plane", "core\\models\\plane.fbx", "Mesh");
			}
			if (ImGui::Selectable("Sphere"))
			{
				CreateEntityWithMesh("Sphere", "core\\models\\sphere.fbx", "Sphere");
			}
			ImGui::Separator();

			if (ImGui::Selectable("Point Light")) 
			{
				Entity* e = s_hierarchy.scene->AddEntity(-1, "Point Light");
				e->AddComponent<PointLight>(prep_editor.rndr->CreatePointLight(e->ID));
			}
			if (ImGui::Selectable("Directional Light")) 
			{
				Entity* e = s_hierarchy.scene->AddEntity(-1, "Directional Light");
				e->AddComponent<DirectionalLight>(prep_editor.rndr->CreateDirectionalLight());
			}
			if (ImGui::Selectable("Spot Light"))
			{
				Entity* e = s_hierarchy.scene->AddEntity(-1, "Spot Light");
				e->AddComponent<SpotLight>(prep_editor.rndr->CreateSpotLight(e->ID));
			}
			if (ImGui::Selectable("Reflection Probe"))
			{
				Entity* e = s_hierarchy.scene->AddEntity(-1, "Reflection Probe");
				e->AddComponent<ReflectionProbe>(prep_editor.rndr->CreateReflectionProbe(e->ID));
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Script"))
		{
			if (ImGui::MenuItem("Recompile Scripts")){
				s_hierarchy.scene->PushRequest(SR_RECOMPILE_SCRIPTS);
			}
			if (ImGui::MenuItem("Edit Script")) {
				ImGuiFileDialog::Instance()->OpenModal("LoadScrModel", "Open Script", ".lua\0.LUA\0\0", ".");
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Project Browser")) {
				prj_browser.isOn = true;
			}
			if (ImGui::MenuItem("Project Settings")) {
				prg_settings.isOn = true;
			}
			if (ImGui::MenuItem("Scene")) {
				
			}
			if (ImGui::MenuItem("Preperties")) {
				
			}
			if (ImGui::MenuItem("Scene Settings")) {

			}
			ImGui::Separator();
			if (ImGui::MenuItem("Console")) {
				db_editor.is_open = true;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Grass"))
		{
			if (!s_hierarchy.scene->edit_grass && ImGui::MenuItem("Enable"))
				s_hierarchy.scene->edit_grass = true;
			else if (s_hierarchy.scene->edit_grass  && ImGui::MenuItem("Disable"))
				s_hierarchy.scene->edit_grass = false;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Online Docs")) {
			}
			if (ImGui::MenuItem("Community Forum")) {
			}
			if (ImGui::MenuItem("Contact Us")) {
			}
			ImGui::Separator();
			if (ImGui::MenuItem("About ")) {
			
			}
			ImGui::EndMenu();
		}
		ImGui::SameLine(0, ImGui::GetWindowWidth() - 275);

		ImGui::EndMainMenuBar();
	}

	if (ImGuiFileDialog::Instance()->FileDialog("SaveScnModel"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk == true)
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			// action
			s_hierarchy.scene->SaveScene(filePathName);
			eraseSubStr(filePathName, filePath);
			s_hierarchy.scene->name = filePathName;
		}
		// close
		ImGuiFileDialog::Instance()->CloseDialog("SaveScnModel");
	}

	if (ImGuiFileDialog::Instance()->FileDialog("LoadScnModel"))
	{
		// action if OK
		if (ImGuiFileDialog::Instance()->IsOk == true)
		{
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilepathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			// action
			s_hierarchy.scene->LoadScene(filePathName);
			eraseSubStr(filePathName, filePath);
			s_hierarchy.scene->name = filePathName;
		}
		// close
		ImGuiFileDialog::Instance()->CloseDialog("LoadScnModel");
	}

	ImVec2 window_pos = ImVec2((0 & 1) ? (viewport->Pos.x + viewport->Size.x - 0) : (viewport->Pos.x + 0), (0 & 2) ? (viewport->Pos.y + viewport->Size.y - 48) : (viewport->Pos.y + 19));
	ImVec2 window_pos_pivot = ImVec2((0 & 1) ? 1.0f : 0.0f, (0 & 2) ? 1.0f : 0.0f);

	ImGui::SetNextWindowPos(ImVec2(window_pos.x, window_pos.y + 1.0f), ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::SetNextWindowSizeConstraints(ImVec2((float)SCR_weight, 35), ImVec2((float)SCR_weight, 35));

	ImGui::Begin("Tool_BAR", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::PushFont(tex);
	ImGui::Button("{"); ImGui::SameLine();
	ImGui::Button("3"); ImGui::SameLine();
	ImGui::Button("J"); ImGui::SameLine();
	ImGui::Text("|"); ImGui::SameLine();

	ImGui::Button("P"); ImGui::SameLine();

	if (mCurrentGizmoOperation == ImGuizmo::OPERATION::TRANSLATE)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.23f, 0.40f, 0.24f, 1.00f));
		ImGui::Button("F"); ImGui::SameLine();
		ImGui::PopStyleColor();
	}
	else
	{
		if (ImGui::Button("F"))
			mCurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	}
	ImGui::SameLine();

	if (mCurrentGizmoOperation == ImGuizmo::OPERATION::ROTATE)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.23f, 0.40f, 0.24f, 1.00f));
		ImGui::Button("G"); ImGui::SameLine();
		ImGui::PopStyleColor();
	}
	else
	{
		if (ImGui::Button("G"))
			mCurrentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
	}
	ImGui::SameLine();

	if (mCurrentGizmoOperation == ImGuizmo::OPERATION::SCALE)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.23f, 0.40f, 0.24f, 1.00f));
		ImGui::Button("H"); ImGui::SameLine();
		ImGui::PopStyleColor();
	}
	else
	{
		if (ImGui::Button("H"))
			mCurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;
	}
	ImGui::SameLine();

	ImGui::Text("|"); ImGui::SameLine();

	if (mCurrentGizmoMode == ImGuizmo::MODE::WORLD)
	{
		if (ImGui::Button("4"))
			mCurrentGizmoMode = ImGuizmo::MODE::LOCAL;
	}
	else
	{
		if (ImGui::Button("t"))
			mCurrentGizmoMode = ImGuizmo::MODE::WORLD;
	}
	ImGui::SameLine();

	ImGui::Text("|"); ImGui::SameLine();

	if (scripts_board)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.23f, 0.40f, 0.24f, 1.00f));
		if (ImGui::Button("y"))
			scripts_board = false;
		ImGui::PopStyleColor();
	}
	else
		if (ImGui::Button("y"))
			scripts_board = true;

	ImGui::SameLine();

	ImGui::Text("|"); ImGui::SameLine();

	ImGui::Button("w"); ImGui::SameLine();

	if (s_hierarchy.scene->game_view)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.23f, 0.40f, 0.24f, 1.00f));
		if (ImGui::Button("6"))
			s_hierarchy.scene->game_view = false;
		ImGui::PopStyleColor();
	}
	else
		if (ImGui::Button("6"))
			s_hierarchy.scene->game_view = true;
	
	ImGui::SameLine();

	if (s_hierarchy.scene->is_playing)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.23f, 0.40f, 0.24f, 1.00f));
		if (ImGui::Button("C")) {
			s_hierarchy.scene->PushRequest(SR_PLAY_SCENE);
		}
		ImGui::PopStyleColor();
		
	}
	else if (ImGui::Button("C")) s_hierarchy.scene->PushRequest(SR_PLAY_SCENE);
	
	ImGui::SameLine();
	ImGui::PopFont();
	if (s_hierarchy.scene->is_playing) {
		ImGui::SameLine();
		ImGui::Text("Playing");
	}
	ImGui::End();
}

void Game_Editor::DownBar()
{
	ImVec2 window_pos = ImVec2((2 & 1) ? (viewport->Pos.x + viewport->Size.x) : (viewport->Pos.x), (2 & 2) ? (viewport->Pos.y + viewport->Size.y - 18) : (viewport->Pos.y + 18));
	ImVec2 window_pos_pivot = ImVec2((2 & 1) ? 1.0f : 0.0f, (2 & 2) ? 1.0f : 0.0f);

	ImGui::SetNextWindowPos(ImVec2(window_pos.x, window_pos.y + 23), ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::SetNextWindowSizeConstraints(ImVec2((float)SCR_weight, 0), ImVec2((float)SCR_weight, 0));

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.15f, 0.15f, 0.15f, 1.f));
	ImGui::Begin("DebugInfo", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
	ImGui::PopStyleColor();
}

void Game_Editor::DockSpaces()
{
	ImVec2 win01_pos, win01_size, win02_size;

	float a1;
	ImVec2 window_pos = ImVec2((1 & 1) ? (viewport->Pos.x + viewport->Size.x - 0) : (viewport->Pos.x + 0), (1 & 2) ? (viewport->Pos.y + viewport->Size.y - 48) : (viewport->Pos.y + 19));
	ImVec2 window_pos_pivot = ImVec2((1 & 1) ? 1.0f : 0.0f, (1 & 2) ? 1.0f : 0.0f);

	ImGui::SetNextWindowPos(ImVec2(window_pos.x, window_pos.y + 35), ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::SetNextWindowSizeConstraints(ImVec2(10, (float)SCR_height - 80), ImVec2(700, (float)SCR_height - 80));
	//bool opn = true;

	ImGui::Begin("Dock_01", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
	a1 = ImGui::GetWindowSize().x;
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace0");
	ImGui::DockSpace(dockspace_id, ImVec2(0, 0));
	win02_size = ImGui::GetWindowSize();
	ImGui::End();

	window_pos = ImVec2((0 & 1) ? (viewport->Pos.x + viewport->Size.x - 0) : (viewport->Pos.x + 0), (0 & 2) ? (viewport->Pos.y + viewport->Size.y - 48) : (viewport->Pos.y + 19));
	window_pos_pivot = ImVec2((0 & 1) ? 1.0f : 0.0f, (0 & 2) ? 1.0f : 0.0f);

	//viewPortPos = ImVec2(window_pos.x, window_pos.y + 41);
	MainViewport->left_pos = window_pos.x;
	MainViewport->top_pos = window_pos.y + 41;

	ImGui::SetNextWindowPos(ImVec2(window_pos.x, window_pos.y + 35), ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::SetNextWindowSizeConstraints(ImVec2(10, (float)SCR_height - 80), ImVec2(700, (float)SCR_height - 80));

	bool opn1 = true;
	ImGui::Begin("Dock_02", &opn1, ImGuiWindowFlags_NoTitleBar);
	float rw = ImGui::GetWindowSize().x;
	ImGuiID dockspace_id2 = ImGui::GetID("MyDockSpace1");
	ImGui::DockSpace(dockspace_id2, ImVec2(0, 0));
	win01_pos = ImGui::GetWindowPos();
	win01_size = ImGui::GetWindowSize();
	ImGui::End();

	if (scripts_board)
	{
		ImGui::SetNextWindowPos(ImVec2(win01_pos.x + win01_size.x, win01_pos.y), ImGuiCond_Always, window_pos_pivot);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::SetNextWindowSize(ImVec2(SCR_weight - win01_size.x - win02_size.x, win01_size.y));
	 
		ImGui::Begin("Dock_033 ForScripts", &scripts_board, ImGuiWindowFlags_NoTitleBar || ImGuiWindowFlags_NoResize);
		ImGui::Text("");
		ImGui::SameLine(0, ImGui::GetWindowWidth() / 2 - 90);
		ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 0.6f), "Quick scripts editing here");

		ImGuiID dockspace_id22 = ImGui::GetID("MyDockSpace44");
		ImGui::DockSpace(dockspace_id22, ImVec2(0, 0));
		ImGui::End();
	}

	MainViewport->left_pos = rw;
	MainViewport->top_pos = 0;//26;
	MainViewport->weight = (float)SCR_weight - a1 - rw;
	MainViewport->height = (float)SCR_height - 54;
}
