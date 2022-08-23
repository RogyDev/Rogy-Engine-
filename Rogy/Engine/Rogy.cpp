#include "Rogy.h"

#include "gizmos\debug_gizmos.h"
#include "physics\GLDebugDrawer.h"

DDRenderInterfaceCoreGL* ddGizmos;

// Window weigh and height
int SCR_weight, SCR_height;

// ---------------------------------------------------------------
// ## Editor Camera Movment
// ---------------------------------------------------------------
bool poses_geted;
bool CamCanMove;
double l_xpos, l_ypos;

void FpsCamera(Camera *cam, float dt, GLFWwindow* wind)
{
	if (CamCanMove)
	{
		if (!poses_geted)
		{
			glfwGetCursorPos(wind, &l_xpos, &l_ypos);
			poses_geted = true;
		}
		if (!cam->MouseSeted)
		{
			glfwSetCursorPos(wind, SCR_weight / 2, SCR_height / 2);
			cam->MouseSeted = true;
		}
		glfwSetInputMode(wind, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// Get mouse position
		double xpos, ypos;

		glfwGetCursorPos(wind, &xpos, &ypos);

		// Reset mouse position for next frame
		glfwSetCursorPos(wind, SCR_weight / 2, SCR_height / 2);

		// Compute new orientation
		glm::vec3 rot = cam->transform.Rotation;
		rot.y += cam->MouseSpeed * (float)(SCR_weight / 2 - xpos);
		rot.x += cam->MouseSpeed * (float)(SCR_height / 2 - ypos);
		cam->transform.SetRotation(rot);

		if (glfwGetKey(wind, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			cam->Speed = 50;
		}
		else
		{
			cam->Speed = 15;
		}
		// Move forward
		if (glfwGetKey(wind, GLFW_KEY_W) == GLFW_PRESS)
		{
			//cam.Position += cam.direction * Time::deltaTime * cam.Speed
			cam->transform.Position += cam->transform.direction() * cam->Speed * dt;
		}
		// Move backward
		if (glfwGetKey(wind, GLFW_KEY_S) == GLFW_PRESS)
		{
			cam->transform.Position -= cam->transform.direction()  * cam->Speed * dt;
		}
		// Strafe right
		if (glfwGetKey(wind, GLFW_KEY_D) == GLFW_PRESS)
		{
			cam->transform.Position += cam->transform.right()  * cam->Speed * dt;
		}
		// Strafe left
		if (glfwGetKey(wind, GLFW_KEY_A) == GLFW_PRESS)
		{
			cam->transform.Position -= cam->transform.right() * cam->Speed * dt;
		}
	}
	else
	{
		cam->MouseSeted = false;
		glfwSetInputMode(wind, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		if (poses_geted)
		{
			//Reset mouse position for next frame
			glfwSetCursorPos(wind, l_xpos, l_ypos);
			poses_geted = false;
		}
	}
}

// ---------------------------------------------------------------
// ## Engine
// ---------------------------------------------------------------
Rogy::Rogy() { }
Rogy::~Rogy() { }

void OnWindowResize(GLFWwindow* window, int weigth, int height)
{
	glViewport(0, 0, weigth, height);
	SCR_weight = weigth;
	SCR_height = height;
}

bool Rogy::InitGraphics()
{
	// Initialize the window
	// ----------------------------
	window.StartWindow(SCR_weight, SCR_height, 4, 0);		// Initialize GLFW window with GL/GLSL version 4.0
#ifdef EDITOR_MODE											// Set Window Title
	window.SetWindowTitle(("Rogy Editor - " + mProjectSettings.GameName).c_str());
#else
	window.SetWindowTitle(mProjectSettings.GameName.c_str());
#endif 
	window.SetWindowSizeCallback(OnWindowResize);			// Set Size callback function
	window.GetFramebufferSize(SCR_weight, SCR_height);		// Get Framebuffer size
	window.SetWindowIcon("core/logo.png");					// Change window Icon

	// Set the mouse at the center of the screen
	glfwPollEvents();

	//glfwSwapInterval(1); // Vsync

	// limit frame rate
	//frameRateLimit = 80.0f;

	// Initialize Render Engine
	// ----------------------------
	renderer.Initialize(SCR_weight, SCR_height, window.window, &resManager);

	return true;
}

bool Rogy::Init()
{
	RGetCurrentPath();
	Rogy::getIns(this);

	m_PlayerPrefs.save_path = ProjectResourcesFolder;

	LoadProjectSettings();

	SCR_weight = mProjectSettings.ResolutionWeight;
	SCR_height = mProjectSettings.ResolutionHeight;

	// Initialize Resources
	// ----------------------------
	Init_succes = false;
	
	resManager.Init();
	resManager.scene = &mScene;

	MainViewport.height = (float)SCR_height;
	MainViewport.weight = (float)SCR_weight;
	MainViewport.left_pos = 0.0f;
	MainViewport.top_pos = 0.0f;

	// Initialize Graphics
	// ----------------------------
	if (InitGraphics())
	{
		Init_succes = true;
	}
	else { return false; }

	// Initialize UI
	// ----------------------------
	m_UI.Init();

#ifdef EDITOR_MODE
	EditorMode = true;
	//window.Maximize();
#else
	mScene.game_view = true;
	BeginPlay();
	if (mProjectSettings.DefaultFullScreen)
		window.SetFullScreen(true);
#endif 

	mScene.Root.root = true;

	DebugTool::GetInstance(&m_Debug);

	// Initialize Physics
	// ---------------------------
	m_PhysicsWorld.Init();
	m_PhysicsWorld.mScriptManager = &m_ScriptManager;

	// Initialize Scripting
	// ---------------------------
	m_ScriptManager.debug = &m_Debug;
	m_ScriptManager.ProjectResourcesFolder = ProjectResourcesFolder;
	RecompileScripts();

	// Initialize Audio
	// ---------------------------
	m_Audio.Init();

	// Initialize input
	m_Input.Init(window.window);
	//m_Input.AddAxis("Horizontal", RKey::KEY_D, RKey::KEY_A);
	//m_Input.AddAxis("Vertical"  , RKey::KEY_W, RKey::KEY_S);

	// Initialize Editor
	// ----------------------------
	editor.ProjectResourcesFolder = ProjectResourcesFolder;
	editor.SCR_height = SCR_height;
	editor.SCR_weight = SCR_weight;
	editor.MainViewport = &MainViewport;

	if (editor.Init("#version 130", window.window, true))
	{
		Init_succes = true;

		//if (EditorMode) 
		{
			editor.s_hierarchy.scene = &mScene;
			editor.s_hierarchy.input = &m_Input;

			editor.scn_settings.rndr = &renderer;
			editor.scn_settings._scene = &mScene;

			editor.prep_editor.res = &resManager;
			editor.prep_editor.rndr = &renderer;
			editor.prep_editor.phy_world = &m_PhysicsWorld;
			editor.prep_editor.scrMnger = &m_ScriptManager;

			editor.db_editor.debuger = &m_Debug;

			editor.prg_settings.input = &m_Input;
			editor.prg_settings.rndr = &renderer;
			editor.prg_settings.prj = &mProjectSettings;

			editor.prep_editor.audio_mnger = &m_Audio;
			editor.prep_editor.ui_renderer = &m_UI;

			editor.prj_browser.ProjectDir = ProjectResourcesFolder;
			editor.prj_browser.mScene = &mScene;
			editor.prj_browser.rnder = &renderer;

			editor.mat_editor.renderer = &renderer;
			editor.mat_editor.res = &resManager; 
			editor.prg_settings.rndr = &renderer;
			//editor.prg_settings.ui = &m_UI;
		}
	}
	else return false; 

	return true;
}

// ---------------------------------------------------------------
// ## Play mode
// ---------------------------------------------------------------
bool Rogy::IsPlaying()
{
	return isPlaying;
}
void Rogy::BeginPlay()
{
	mScene.is_playing = true;
	isPlaying = true;
	SaveScene("core\\temp_scene", true);
	m_ScriptManager.BeginGame();
}
void Rogy::StopPlay()
{
	mScene.is_playing = false;
	isPlaying = false;
	LoadScene("core\\temp_scene", true);
}

// ---------------------------------------------------------------
// ## Grass editor
// ---------------------------------------------------------------
void Rogy::GrassEdit()
{
	if (!mScene.edit_grass) return;
	std::vector<GrassComponent*> allGrass = renderer.mGrass.GetComponents();
	GrassComponent* grass = nullptr;
	for (size_t i = 0; i < allGrass.size(); i++)
	{
		if (allGrass[i]->edit)
		{
			grass = allGrass[i];
			break;
		}
	}
	// Grass Edit
	if (grass != nullptr)
	{
		glm::vec3 ray_Start, ray_Dir, grass_Pos;
		int mouse_X = int(m_Input.GetMouseXPos() - MainViewport.left_pos);
		int mouse_Y = int(m_Input.GetMouseYPos() - MainViewport.top_pos - 57);
		m_PhysicsWorld.ScreenPosToWorldRay(mouse_X, mouse_Y
			, (int)MainViewport.weight, (int)MainViewport.height,
			renderer.MainCam.GetViewMatrix(), renderer.MainCam.GetProjectionMatrix()
			, ray_Start, ray_Dir);

		if (m_PhysicsWorld.RaycastHitPoint(renderer.MainCam.transform.Position, ray_Dir, 10000.0f, grass_Pos))
			renderer.AddGuizmoSpheres(grass_Pos, grass->Edit_Raduis);

		if (m_Input.GetMouseButtonDown(0))
		{
			m_PhysicsWorld.updateAABBs();
			if (m_Input.GetKey(RKey::KEY_LSHIFT))
			{
				glm::vec3 rayStart, rayDir, grassPos;
				int mouseX = int(m_Input.GetMouseXPos() - MainViewport.left_pos);
				int mouseY = int(m_Input.GetMouseYPos() - MainViewport.top_pos - 57);
				m_PhysicsWorld.ScreenPosToWorldRay(mouseX, mouseY
					, (int)MainViewport.weight, (int)MainViewport.height,
					renderer.MainCam.GetViewMatrix(), renderer.MainCam.GetProjectionMatrix()
					, rayStart, rayDir);

				if (m_PhysicsWorld.RaycastHitPoint(renderer.MainCam.transform.Position, rayDir, 10000.0f, grassPos))
					grass->RemoveBladesInRange(grassPos + glm::vec3(0.0f, grass->size.y, 0.0f));
			}
			else {
				int aRaduis = grass->Edit_Raduis;
				for (size_t i = 0; i < grass->Edit_amount; i++)
				{
					glm::vec3 rayStart, rayDir, grassPos;
					int mouseX = int(m_Input.GetMouseXPos() - MainViewport.left_pos);
					int mouseY = int(m_Input.GetMouseYPos() - MainViewport.top_pos - 57);
					mouseX += Random::RangeI(-aRaduis, aRaduis);
					mouseY += Random::RangeI(-aRaduis, aRaduis);
					m_PhysicsWorld.ScreenPosToWorldRay(mouseX, mouseY
						, (int)MainViewport.weight, (int)MainViewport.height,
						renderer.MainCam.GetViewMatrix(), renderer.MainCam.GetProjectionMatrix()
						, rayStart, rayDir);

					if (m_PhysicsWorld.RaycastHitPoint(renderer.MainCam.transform.Position, rayDir, 10000.0f, grassPos))
						grass->AddGrassBlade(grassPos + glm::vec3(0.0f, grass->size.y, 0.0f));
				}
			}
		}
	}
}

CDebugDraw* debugDrawer;
Entity *anim, *wpn, *mag;
// ---------------------------------------------------------------
// ## Update
// ---------------------------------------------------------------
void Rogy::StartUp()
{
#ifdef EDITOR_MODE
	m_icons.emplace_back(EditorIcon::EI_PLight, "core\\editor\\IconPointLight.png", resManager);
	m_icons.emplace_back(EditorIcon::EI_SLight, "core\\editor\\IconSpotLight.png", resManager);
	m_icons.emplace_back(EditorIcon::EI_DLight, "core\\editor\\IconLight.png", resManager);
	m_icons.emplace_back(EditorIcon::EI_Camera, "core\\editor\\IconCamera.png", resManager);
	m_icons.emplace_back(EditorIcon::EI_Audio, "core\\editor\\IconAudio.png", resManager);
#endif

	/* ----------------------------------------------
	 Set a unique id for each component by registering it
	 Warning : ThisM  reordered; because components types are
	 serialized with theire ID.
	 ----------------------------------------------*/
	REGISTER_COMPONENT(BillboardComponent);
	REGISTER_COMPONENT(SpotLight);
	REGISTER_COMPONENT(DirectionalLight);
	REGISTER_COMPONENT(PointLight);
	REGISTER_COMPONENT(ReflectionProbe);
	REGISTER_COMPONENT(CameraComponent);
	REGISTER_COMPONENT(RendererComponent);
	REGISTER_COMPONENT(RigidBody);
	REGISTER_COMPONENT(NativeScriptComponent);
	REGISTER_COMPONENT(ParticleSystem);
	REGISTER_COMPONENT(RAudioSource);
	REGISTER_COMPONENT(GrassComponent);
	REGISTER_COMPONENT(UIWidget);
	REGISTER_COMPONENT(SkeletalMeshComponent);
	REGISTER_COMPONENT(Terrain);
	REGISTER_COMPONENT(TerrainCollider);

	//uint64_t handle = glGetImageHandleARB()
	/*
	Entity* disp = mScene.AddEntity("Terrain");
	m_Debug.Log("Terrain ID: " + std::to_string(disp->ID));
	Terrain* d = renderer.mDisplacements.AddComponent(disp->ID);
	d->MaxHeight = 150;
	d->Size = 150;
	d->Init(resManager.CreateTexture("Terrain.png", "Terrain.png", false, true));
	d->mat0 = renderer.LoadMaterial("FPSDemo\\Mat1.mat");
	disp->AddComponent<Terrain>(d);
	disp->DontDestroyOnLoad();

	float texWidth = d->mHeightmap->getTexWidth() / 2;
	static std::vector<float> data;
	data.reserve((texWidth*texWidth));
	int idx = 0;
	for (size_t x = 0; x < texWidth; x++)
	{
		for (size_t y = 0; y < texWidth; y++)
		{
			data[idx] = (((float)d->mHeightmap->GetBlue(y * 2, x * 2)) / 256.0f) * d->MaxHeight;
			idx++;
		}
	}
	disp->AddComponent<TerrainCollider>(m_PhysicsWorld.AddTerrainCollider(disp, &data[0], texWidth, d->MaxHeight, d->Size * 2));*/
	/*
	Entity* wpn = mScene.AddEntity("C++Test");
	RendererComponent* wpn_mesh = renderer.m_renderers.AddComponent(wpn->ID);
	wpn->AddComponent<RendererComponent>(wpn_mesh);
	wpn_mesh->mesh = resManager.mMeshs.CreateModel(std::string("core\\models\\cube.fbx"))->GetFirstMesh();
	wpn_mesh->material = renderer.materials.GetMaterialN("");

	NativeScriptComponent* cpp_script = new NativeScriptComponent();
	cpp_script->Bind<CppTest>();
	cpp_script->entid = wpn->ID;
	wpn->AddComponent<NativeScriptComponent>(cpp_script);*/

	/*anim = mScene.AddEntity("Anim");
	SkeletalMeshComponent* mesh = renderer.m_skMeshs.AddComponent(anim->ID);
	mesh->scale = 0.001f;
	anim->AddComponent<SkeletalMeshComponent>(mesh);
	mesh->mesh = resManager.mMeshs.LoadSkeletalModel("res\\mag.fbx");
	mesh->material = renderer.materials.GetMaterialN("");*/

	/*Entity* disp = mScene.AddEntity("Terrain");
	Terrain* d = renderer.mDisplacements.AddComponent(disp->ID);
	d->Init();
	d->mat0 = renderer.LoadMaterial("Mat1.mat");
	d->mat1 = renderer.LoadMaterial("Mat2.mat");
	disp->AddComponent<Terrain>(d);
	disp->DontDestroyOnLoad();*/

	/*renderer.postProc.Use = true;
	anim = mScene.AddEntity("Anim");
	SkeletalMeshComponent* mesh = renderer.m_skMeshs.AddComponent(anim->ID);
	mesh->scale = 0.001f;
	anim->AddComponent<SkeletalMeshComponent>(mesh);
	mesh->mesh = resManager.mMeshs.LoadSkeletalModel("res\\FPS\\AK47.fbx");
	mesh->material = renderer.materials.GetMaterialN("");
	if (mesh->mesh != nullptr) {
		for (size_t i = 0; i < mesh->mesh->num_anims; i++)
			mesh->LoadAnimation(("res\\FPS\\AK47.fbx" + to_string(i)), "res\\FPS\\AK47.fbx", i);
	}
	wpn = mScene.AddEntity("Weapon");
	wpn->SetParent(anim);
	RendererComponent* wpn_mesh = renderer.m_renderers.AddComponent(wpn->ID);
	wpn->AddComponent<RendererComponent>(wpn_mesh);
	wpn_mesh->mesh = resManager.mMeshs.CreateModel(std::string("res\\FPS\\akBody.fbx"))->GetFirstMesh();
	wpn_mesh->material = renderer.materials.GetMaterialN("");*/
}
// ---------------------------------------------------------------
void Rogy::MainLoop()
{
	if (!Init_succes) return;

	m_PhysicsWorld.grv = m_PhysicsWorld.GetGravity();

	StartUp();

	debugDrawer = new CDebugDraw();
	debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	m_PhysicsWorld.dynamicsWorld->setDebugDrawer(debugDrawer);
	
	// Set up the Debug Draw:
	ddGizmos = new DDRenderInterfaceCoreGL();
	dd::initialize(ddGizmos);

	double lasttime = glfwGetTime();

	while (window.active)
	{
#ifdef EDITOR_MODE
		if (EditorMode && !IsPlaying())
			if (!editor.isMouseInEditor())
				this_thread::sleep_for(std::chrono::duration<int>(1));
#endif // EDITOR_MODE

		// Frame rate limiter.
		// ----------------------------------------
		if (frameRateLimit != 0.0f)
		{
			while (glfwGetTime() < lasttime + 1.0 / frameRateLimit)
			{
				continue;
			}
			lasttime += 1.0 / frameRateLimit;
		}
		// per-frame time logic
		// ----------------------------------------
		float currentFrame = (float)glfwGetTime();
		time = (float)glfwGetTime();
		renderer.CurrentTime = time;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		FPS = (int)(1.0f / deltaTime);
		renderer.fps = FPS;
		m_PhysicsWorld.IsPlaying = IsPlaying();
		PhyDeltaTime = m_PhysicsWorld.TimeStep;

		m_Input.Update(deltaTime);
		// Prepare for the new frame
		// ----------------------------------------
#ifdef EDITOR_MODE
		if (!mScene.game_view) {
			CamCanMove = glfwGetMouseButton(window.window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
			if (ImGui::IsAnyWindowHovered())
				CamCanMove = false;
			else GrassEdit();
			FpsCamera(&renderer.MainCam, deltaTime, window.window);
		}
		else if (isPlaying && m_Input.GetKeyDown(RKey::KEY_ESCAPE))
			m_Input.SetCursor(true);
		
		renderer.OnViewportResize((int)MainViewport.left_pos, (int)MainViewport.top_pos, (int)MainViewport.weight, (int)MainViewport.height);
		m_UI.SetScreenSize((int)MainViewport.left_pos, (int)MainViewport.top_pos + 8, (int)MainViewport.weight, (int)MainViewport.height);
#else
		renderer.OnViewportResize(0, 0, SCR_weight, SCR_height);
		m_UI.SetScreenSize(0, 0, SCR_weight, SCR_height);
#endif // EDITOR_MODE

		/* Serialization operations are unreliable, sometimes can cause errors
		   For example : corrupted files etc...	*/
		QueueSpawnList();
		/*try { QueueSpawnList(); }
		catch (cereal::Exception ex) { 
			std::cout << "QueueSpawnList : " << ex.what() << endl; 
			mScene.spawn_requests.clear();
			mScene.m_requests.clear();
		}

		/* Step the physics simulation */
		m_PhysicsWorld.update();
		if (IsPlaying())
			m_PhysicsWorld.StepSimulation(deltaTime);

		if (IsPlaying())
			m_ScriptManager.OnPreTick(deltaTime);

		/* Update scene entities */
		mScene.Root.Update(deltaTime);
		if (IsPlaying())
			m_ScriptManager.OnTick(deltaTime);
		UpdateEntity(&mScene.Root);
		/* Update camera */
		renderer.UpdateGameCamera();
		PushAllRenders();
		/* Update Audio */
		m_Audio.SetListenerPosition(renderer.MainCam.transform.Position);
		m_Audio.SetListenerForward(renderer.MainCam.transform.forward());
		m_Audio.SetListenerUp(renderer.MainCam.transform.up());
		m_Audio.Update();
		/* Update Scripts */
		m_ScriptManager.Update();
		if (IsPlaying())
			m_ScriptManager.OnTick(deltaTime);

		m_Input.ResetMouseDelta();

		// Render Frame
		// ----------------------------------------
		if (renderer.BakeLighting)
			lmBaked = true;

		//renderer.UpdateCamera();

#ifdef EDITOR_MODE

		if (!mScene.game_view && !gizHovred && !mScene.edit_grass && !ImGui::GetIO().WantCaptureMouse)
		{
			if (m_Input.GetMouseButtonDown(0) /*&& m_Input.GetKey(RKey::KEY_LCTRL)*/)
			{
				bool diselect_others = !m_Input.GetKey(RKey::KEY_LCTRL);

				int mouseX = int(m_Input.GetMouseXPos() - MainViewport.left_pos);
				int mouseY = int(m_Input.GetMouseYPos() - MainViewport.top_pos - 57);
				if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)MainViewport.weight && mouseY < MainViewport.weight) {
					glm::vec4 data = renderer.Get3dPosition(mouseX, mouseY);
					EnttID entID = (EnttID)data.w;
					Entity* s_ent = mScene.FindEntity(entID);
					if (s_ent != nullptr) 
					{
						if(s_ent->parent != nullptr && !s_ent->parent->IsRoot() && editor.s_hierarchy.GetSelected() != s_ent->parent->ID)
							editor.s_hierarchy.SetSelection(s_ent->parent->ID, diselect_others, true);
						else
							editor.s_hierarchy.SetSelection(entID, diselect_others, true);
					}
				}
			}
		}

		if (!mScene.game_view && !gizHovred)
		{
			if (m_Input.GetMouseButton(0) && mScene.editDisp != 8888)
			{
				int mouseX = int(m_Input.GetMouseXPos() - MainViewport.left_pos);
				int mouseY = int(m_Input.GetMouseYPos() - MainViewport.top_pos - 57);
				if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)MainViewport.weight && mouseY < MainViewport.weight) {
					glm::vec4 data = renderer.Get3dPosition(mouseX, mouseY);

					std::vector<Terrain*> disps = renderer.mDisplacements.GetComponents();
					for (size_t i = 0; i < disps.size(); i++)
					{
						if (disps[i]->entid == mScene.editDisp)
						{
							Terrain* disp = disps[i]; 
							if (disp->useOneHeight == true)
							{
								disp->useOneHeight = false;
								disp->SetHeightAll(disp->SameHeight);
							}
							else 
							{
								float amount = disps[i]->Intensity;
								if (m_Input.GetKey(RKey::KEY_LSHIFT)) amount = -disps[i]->Intensity;

								if (disps[i]->edit_alpha)
									disp->SetAlpha(glm::vec3(data.x, data.y, data.z), disps[i]->Raduis, amount);
								else
									disp->SetHeight(glm::vec3(data.x, data.y, data.z), disps[i]->Raduis, amount);
							}
							break;
						}
					}
				}
			}
		}

#endif

		renderer.RenderFrame(deltaTime);
		
#ifdef EDITOR_MODE
		// If the Lightmaps where baked last frame set the texture of every mesh to its lightmap texture.
		if (renderer.isLightBakingSucceed() && lmBaked)
		{
			lmSetIndex = 0;
			SetEntitiesLightmapPaths(mScene.GetRoot());
			lmBaked = false;
		}
		
		if (EditorMode && !mScene.game_view)
		{
			ddGizmos->mvpMatrix = renderer.MainCam.GetProjectionMatrix() * renderer.MainCam.GetViewMatrix();
			ddGizmos->scr_w = (int)MainViewport.weight;
			ddGizmos->scr_h = (int)MainViewport.height;
			renderer.Grid = (EditorMode && mScene.show_grid);
			if (EditorMode) {

				//if (mScene.show_grid)
					//dd::xzSquareGrid(-50.0f, 50.0f, -1.0f, 1.0f, dd::colors::Gray);
				// Debug physics
				//m_PhysicsWorld.dynamicsWorld->debugDrawWorld();
				for (size_t i = 0; i < debugDrawer->LINES.size(); i++)
				{
					CDebugDraw::_LINE l = debugDrawer->LINES[i];
					const ddVec3 mfrom = { l.from.getX(), l.from.getY(), l.from.getZ() };
					const ddVec3 mto = { l.to.getX(), l.to.getY(), l.to.getZ() };
					dd::line(mfrom, mto, dd::colors::Green);
				}
				debugDrawer->LINES.clear();
			}
			dd::flush();
		}
		RenderGuizmos();
#endif
		
		renderer.EndFrame();
		
		if(mScene.game_view)
			m_UI.RenderBlurFrames(renderer.postProc.BluredScreen.textureColorbuffer);

		// Render UI
		// ----------------------------------------
		//m_UI.Update();
		if (!editor.prg_settings.loadFont.empty())
		{
			m_UI.LoadFont(editor.prg_settings.loadFont, editor.prg_settings.lfs);
			ImGui::GetIO().Fonts->Build();
			ImGui_ImplOpenGL3_CreateDeviceObjects();
			editor.prg_settings.loadFont = "";
		}
		RenderUI();

		// Game UI
		// ----------------------------------------
#ifdef EDITOR_MODE
		if(mScene.game_view)
			m_UI.RenderHUD(deltaTime);
		editor.render();
#else
		m_UI.RenderHUD(deltaTime);
		m_UI.EndFrame();
#endif
		//  Swap buffers
		// ----------------------------------------
		m_Input.Clear();
		window.SwapBuffersAndPollEvents();
	}
	//  Clear
	// ----------------------------------------
	editor.ShutDown();
	delete debugDrawer;
	delete ddGizmos;
	m_ScriptManager.Close();
	Clear();

	Quiting = true;
	SaveProjectSettings();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

// ---------------------------------------------------------------
// ## Billboards 
// ---------------------------------------------------------------
void Rogy::PushAllRenders()
{
	glm::vec3 camPos = renderer.MainCam.transform.Position;
	for (size_t i = 0; i < renderer.r_billboards.size(); i++)
	{
		BillboardComponent* bb = renderer.r_billboards[i];
		if (bb->depth_test)
			renderer.AddBillbroadInfo(bb->pos, bb->size, bb->tex_path.c_str(), true, bb->color, bb->use_tex_as_mask);
		else
		{
			if (bb->sun_source)
			{
				glm::vec3 lpos = bb->dir + camPos;
				vec3 lightDir = glm::normalize(lpos - camPos);
				if (!m_PhysicsWorld.RayTest(camPos, lightDir, 99999.9f))
					renderer.AddBillbroadInfo(lpos, bb->size, bb->tex_path.c_str(), false, bb->color, bb->use_tex_as_mask);
			}
			else
			{
				float range = glm::distance(camPos, bb->pos);
				vec3 lightDir = glm::normalize(bb->pos - camPos);
				if (!m_PhysicsWorld.RayTest(camPos, lightDir, range))
					renderer.AddBillbroadInfo(bb->pos, bb->size, bb->tex_path.c_str(), false, bb->color, bb->use_tex_as_mask);
			}
		}
	}
}

// ---------------------------------------------------------------
// ## Update Entities
// ---------------------------------------------------------------
void Rogy::UpdateEntity(Entity* ent)
{
	Transform& transform = ent->transform;
	bool ChangedTransform = transform.transformation_changed;
	transform.transformation_changed = false;

	ent->is_Selected = editor.s_hierarchy.IsSelected(ent->ID);

	bool active = ent->Active;
	for (size_t i = 0; i < ent->m_components.size(); i++)
	{
		Component* comp = ent->m_components[i];
		// Push Renderer component to the renderer
		if (Component::IsComponentType<RendererComponent>(comp))
		{
			bool issk = false;
			glm::mat4 mmt;
			if (ent->parent->HasComponent<SkeletalMeshComponent>())
			{
				mmt = ent->parent->GetComponent<SkeletalMeshComponent>()->animator.wp;
				glm::vec3 translation, rotation, scale;
				LMath::DecomposeTransform(ent->parent->GetComponent<SkeletalMeshComponent>()->animator.wp, translation, rotation, scale);
				ent->SetTranslation( glm::vec3(ent->parent->GetComponent<SkeletalMeshComponent>()->animator.wp[3]));
				ent->SetTranslation(translation);
				ent->SetRotation(rotation);
				ent->transform.UpdateTransform();
			}
			//if (!active) continue;
			RendererComponent* rc = Component::QuickCast<RendererComponent>(comp);
			if (rc->mesh) {
				rc->enabled = ent->Active;
				rc->IsStatic = ent->Static;
				rc->drawOutline = ent->is_Selected;

				if (ChangedTransform) {
					rc->position = transform.GetWorldPosition();
					rc->transform = transform.GetTransform();
					rc->bbox = rc->mesh->bbox;
					//rc->bbox.Transform(rc->transform, rc->position);
					rc->bbox.Transform(rc->transform, transform.up(), transform.right(), transform.forward(), transform.Scale);
				}

				//m_nav.m_mesh = rc->mesh;
				//m_nav.maxx = rc->bbox.BoxMin;
				//m_nav.minx = rc->bbox.BoxMax;

				if (!mScene.game_view && ent->is_Selected && !rc->bbox.useRaduis) {
					// Axis-aligned bounding box:
					const ddVec3 bbMins = { rc->bbox.BoxMin.x, rc->bbox.BoxMin.y, rc->bbox.BoxMin.z };
					const ddVec3 bbMaxs = { rc->bbox.BoxMax.x, rc->bbox.BoxMax.y, rc->bbox.BoxMax.z };
					const ddVec3 bbCenter = {(bbMins[0] + bbMaxs[0]) * 0.5f,(bbMins[1] + bbMaxs[1]) * 0.5f,(bbMins[2] + bbMaxs[2]) * 0.5f};
					dd::aabb(bbMins, bbMaxs, dd::colors::Gray);
				}
				
				if (ent->parent->HasComponent<SkeletalMeshComponent>())
				{
					renderer.PushRender(rc->mesh, rc->material, glm::scale( ent->parent->GetComponent<SkeletalMeshComponent>()->animator.wp * rc->transform, glm::vec3(ent->parent->GetComponent<SkeletalMeshComponent>()->scale)), rc->bbox, rc->CastShadows, rc->position, rc->IsStatic, rc->lightmapPath, ent->ID);
				}

				/*if(rc->material->cutout)
					renderer.PushCutoutRender(rc->mesh, rc->material, rc->transform, rc->bbox, rc->CastShadows, rc->position, rc->IsStatic, rc->lightmapPath, ent->ID);
				else
					renderer.PushRender(rc->mesh, rc->material, rc->transform, rc->bbox, rc->CastShadows, rc->position, rc->IsStatic, rc->lightmapPath, ent->ID);*/
				//renderer.m_RenderBuffer.PushRenderer(rc->material->id, ent->ID);
			}
			continue;
		}

		if (Component::IsComponentType<Terrain>(comp))
		{
			Terrain* disp = Component::QuickCast<Terrain>(comp);
			disp->model = transform.GetTransform();
			continue;
		}
		if (Component::IsComponentType<RigidBody>(comp))
		{
			RigidBody* rb = Component::QuickCast<RigidBody>(comp);
			
			if (transform.m_Last_Transform != transform.m_Transform)
				rb->rigidBody->activate();

			btTransform& tr = rb->GetTransform();
			btVector3& ve = tr.getOrigin();
			btQuaternion& qu = tr.getRotation();
			quat q = quat(qu.getW(), qu.getX(), qu.getY(), qu.getZ());
			transform.SetRotation(q);
			//transform.SetPosition(ve.getX(), ve.getY(), ve.getZ());
			transform.SetPosition(glm::vec3(ve.getX(), ve.getY(), ve.getZ()) - rb->mOffset);

			if(ent->is_Selected && !mScene.game_view)
				m_PhysicsWorld.dynamicsWorld->debugDrawObject(rb->rigidBody->getWorldTransform(), rb->rigidBody->getCollisionShape(), btVector3(1, 1, 1));
			continue;
		}

		if (Component::IsComponentType<PointLight>(comp))
		{
			PointLight* point_light = Component::QuickCast<PointLight>(comp);
			point_light->Active = active;
			point_light->Static = ent->Static;
			point_light->Position = transform.GetWorldPosition();

			if (!mScene.game_view && ent->is_Selected)
			{
				vec3 cen = transform.GetWorldPosition();
				float rad = point_light->Raduis;
				const ddVec3 bbCenter = { cen.x, cen.y, cen.z };
				const ddVec3 xbbnorm = { 1, 0, 0 };
				const ddVec3 ybbnorm = { 0, 1, 0 };
				const ddVec3 zbbnorm = { 0, 0, 1 };

				dd::circle(bbCenter, xbbnorm, dd::colors::Gray, rad, 40);
				dd::circle(bbCenter, ybbnorm, dd::colors::Gray, rad, 40);
				dd::circle(bbCenter, zbbnorm, dd::colors::Gray, rad, 40);
			}
			continue;
		}

		if (Component::IsComponentType<DirectionalLight>(comp))
		{
			DirectionalLight* dir_light = Component::QuickCast<DirectionalLight>(comp);
			dir_light->Active = active;
			dir_light->Direction = transform.forward();
			continue;
		}

		if (Component::IsComponentType<SpotLight>(comp))
		{
			SpotLight* spot_light = Component::QuickCast<SpotLight>(comp);
			spot_light->Active = active;
			spot_light->Static = ent->Static;
			spot_light->Direction = transform.forward();
			spot_light->Position = transform.GetWorldPosition();
			continue;
		}
		if (Component::IsComponentType<ReflectionProbe>(comp))
		{
			ReflectionProbe* ref_probe = Component::QuickCast<ReflectionProbe>(comp);
			ref_probe->Active = active;
			ref_probe->Position = transform.GetWorldPosition();

			if (ref_probe->use_scale)
			{
				ref_probe->box.BoxMin = -transform.Scale;
				ref_probe->box.BoxMax = transform.Scale;
			}

			if (!mScene.game_view && ent->is_Selected)
			{
				// Axis-aligned bounding box:
				const ddVec3 bbMins = { ref_probe->GetBBox().BoxMin.x
					, ref_probe->GetBBox().BoxMin.y
					, ref_probe->GetBBox().BoxMin.z };

				const ddVec3 bbMaxs = { ref_probe->GetBBox().BoxMax.x
					, ref_probe->GetBBox().BoxMax.y
					, ref_probe->GetBBox().BoxMax.z };

				const ddVec3 bbCenter = {
					(bbMins[0] + bbMaxs[0]) * 0.5f,
					(bbMins[1] + bbMaxs[1]) * 0.5f,
					(bbMins[2] + bbMaxs[2]) * 0.5f
				};
				dd::aabb(bbMins, bbMaxs, dd::colors::Gray);


				renderer.AddGuizmoBox(ref_probe->Position, ref_probe->GetBBox().BoxMin, ref_probe->GetBBox().BoxMax);
			}
			continue;
		}

		if (Component::IsComponentType<BillboardComponent>(comp))
		{
			BillboardComponent* bb = Component::QuickCast<BillboardComponent>(comp);
			bb->pos = transform.GetWorldPosition();
			bb->dir = transform.forward();
			continue;
		}
		if (Component::IsComponentType<ParticleSystem>(comp))
		{
			ParticleSystem* bb = Component::QuickCast<ParticleSystem>(comp);
			bb->TargetPos = transform.GetWorldPosition();
			bb->Direction = transform.forward();
			continue;
		}
		// Update Camera info
		if (Component::IsComponentType<CameraComponent>(comp))
		{
			CameraComponent* cam = Component::QuickCast<CameraComponent>(comp);
			cam->position = transform.GetWorldPosition();
			cam->right = transform.right();
			cam->direction = transform.forward();
			cam->up = transform.up();
			continue;
		}
		if (Component::IsComponentType<RAudioSource>(comp))
		{
			RAudioSource* audio = Component::QuickCast<RAudioSource>(comp);
			audio->SetPosition(transform.GetWorldPosition());
			continue;
		}
		if (Component::IsComponentType<SkeletalMeshComponent>(comp))
		{
			SkeletalMeshComponent* anim = Component::QuickCast<SkeletalMeshComponent>(comp);
			anim->position = ent->transform.GetWorldPosition();
			anim->transform = transform.GetTransform();

			continue;
		}
		if (Component::IsComponentType<NativeScriptComponent>(comp))
		{
			NativeScriptComponent* nsc = Component::QuickCast<NativeScriptComponent>(comp);
			if (!nsc->Instance)
			{
				nsc->Instance = nsc->InstantiateScript();
				nsc->Instance->m_entity = ent;
				nsc->Instance->OnCreate();
			}
			if(IsPlaying())
				nsc->Instance->OnUpdate(deltaTime);
			continue;
		}
		if (Component::IsComponentType<TerrainCollider>(comp))
		{
			TerrainCollider* mterrain = Component::QuickCast<TerrainCollider>(comp);
			glm::vec3 s = ent->transform.GetLocalScale();
			//mterrain->shape->setLocalScaling(btVector3(s.x, s.y, s.z));
			if (ent->Active && ent->is_Selected && !mScene.game_view)
				m_PhysicsWorld.dynamicsWorld->debugDrawObject(mterrain->body->getWorldTransform(), mterrain->body->getCollisionShape(), btVector3(1, 1, 1));
			continue;
		}
	}

	for (size_t i = 0; i < ent->Children.size(); i++)
	{
		UpdateEntity(ent->Children[i]);
	}
}

// ---------------------------------------------------------------
// ## Spawn entities, load/save/clear scene
// ---------------------------------------------------------------
void Rogy::QueueSpawnList()
{
	renderer.use_GameView = mScene.game_view;

	if(!mainSceneLoaded && !mProjectSettings.MainScenePath.empty())
	{
		LoadScene(mProjectSettings.MainScenePath.c_str());
		mainSceneLoaded = true;
	}

	for (size_t i = 0; i < mScene.m_requests.size(); i++)
	{
		if (mScene.m_requests[i] == SR_RECOMPILE_SCRIPTS)
		{
			RecompileScripts();
			break;
		}
		else if (mScene.m_requests[i] == SR_PAST_COPY)
		{
			SpawnEntity(std::string("savedCopy"), true);
			break;
		}
		else if (mScene.m_requests[i] == SR_PLAY_SCENE)
		{
			if (IsPlaying())
				StopPlay();
			else
				BeginPlay();
			break;
		}
		else if (mScene.m_requests[i] == SR_QUIT_GAME)
		{
			window.CloseWindow();
			break;
		}
		else if (mScene.m_requests[i] == SR_NEW_SCENE)
		{
			ClearScene();
			mScene.name = "";
			mScene.path = "";
			mScene.Root.name = "New Scene";
			break;
		}
		else if (mScene.m_requests[i] == SR_LOAD_SCENE)
		{
			if(!IsPlaying())
				m_Debug.Log(("Loading: " + mScene.name));
			LoadScene(mScene.path.c_str());
			break;
		}
		else if (mScene.m_requests[i] == SR_SAVE_SCENE)
		{
			SaveScene(mScene.path.c_str());
			if (!IsPlaying())
				m_Debug.Log(("Saved: " + mScene.name));
			break;
		}
		else if (mScene.m_requests[i] == SR_SPAWN_AT_MOUSE)
		{
			for (size_t j= 0; j < editor.prj_browser.dir_files.size(); j++)
			{
				if (editor.prj_browser.dir_files[j].is_selected && editor.prj_browser.dir_files[j].is_prefab)
				{
					m_PhysicsWorld.updateAABBs();

					glm::vec3 rayStart, rayDir, daMousePos;
					int mouseX = int(m_Input.GetMouseXPos() - MainViewport.left_pos);
					int mouseY = int(m_Input.GetMouseYPos() - MainViewport.top_pos - 57);
					m_PhysicsWorld.ScreenPosToWorldRay(mouseX, mouseY
						, (int)MainViewport.weight, (int)MainViewport.height,
						renderer.MainCam.GetViewMatrix(), renderer.MainCam.GetProjectionMatrix()
						, rayStart, rayDir);

					if (m_PhysicsWorld.RaycastHitPoint(renderer.MainCam.transform.Position, rayDir, 10000.0f, daMousePos))
					{
						Entity* eid = SpawnEntity((editor.prj_browser.shortcurrent_dir + "\\" + editor.prj_browser.dir_files[j].name));
						eid->SetTranslation(daMousePos);
						editor.s_hierarchy.SetSelection(eid->ID);
						//renderer.disp.SetHeight(daMousePos, 1.2f, 0.23f);
					}
					break;
				}
			}
			break;
		}
		else if (mScene.m_requests[i] == SR_EDIT_DISP)
		{
			
			break;
		}
	}

	for (size_t i = 0; i < mScene.spawn_requests.size(); i++)
	{
		if (mScene.spawn_requests[i].is_model)
			SpawnModel(mScene.spawn_requests[i].path);
		else
			SpawnEntity(mScene.spawn_requests[i].path);
	}

	mScene.spawn_requests.clear();
	mScene.m_requests.clear();
}

size_t Rogy::LoadAndSpawnEntity(YAML::Node &scnNode, YAML::Node &entNode, Entity* ent, size_t& indx, bool is_scene_root, bool UseCustomTransformation)
{
	bool is_prefab = entNode["IsPrefab"].as<bool>();
	if (is_prefab)
	{
		ent->name = entNode["name"].as<std::string>();
		ent->Active = entNode["Active"].as<bool>();
		ent->Static = entNode["Static"].as<bool>();
		ent->m_CollMode = (RPhyBodyCollMode)(entNode["m_CollMode"].as<int>());
		ent->transform.SerializeLoad(entNode);
		// Load Entity from file.
		// --------------------------------------------------
		std::string& ent_path = entNode["path"].as<std::string>();
		std::ifstream stream(ent_path);

		if (!stream.is_open())
		{
			std::cout << "Unable to load prefab : " << ent_path << std::endl;
#ifdef EDITOR_MODE
			m_Debug.Error("Unable to load prefab : " + ent_path);
#endif
			return indx;
		}

		std::stringstream strStream;
		strStream << stream.rdbuf();
		YAML::Node data;
		data = YAML::Load(strStream.str());

		auto entities = data["Entities"];

		for (size_t i = 0; i < entities.size(); i++)
		{
			YAML::Node& entity = entities[i];
			if (entity["Root"].as<bool>())
				continue;

			i = LoadAndSpawnEntity(entities, entity, ent, i, false, true);
		}

		ent->is_prefab = true;
		ent->path = ent_path;
	}
	else
	{
		ent->tag = entNode["tag"].as<std::string>();
		
		if (!UseCustomTransformation) {
			ent->name = entNode["name"].as<std::string>();
			ent->Active = entNode["Active"].as<bool>();
			ent->Static = entNode["Static"].as<bool>();
			ent->m_CollMode = (RPhyBodyCollMode)(entNode["m_CollMode"].as<int>());
			ent->transform.SerializeLoad(entNode);
		}

		// Serialize components
		size_t comp_count = entNode["comp_count"].as<unsigned int>();
		auto components = entNode["Components"];
		for (auto comp_node : components)
		{
			if (comp_node["DirectionalLight"])
			{
				ent->AddComponent<DirectionalLight>(renderer.CreateDirectionalLight());
				ent->GetComponent<DirectionalLight>()->OnLoad(comp_node["DirectionalLight"]);
			}
			else if (comp_node["PointLight"])
			{
				ent->AddComponent<PointLight>(renderer.CreatePointLight(ent->ID));
				ent->GetComponent<PointLight>()->OnLoad(comp_node["PointLight"]);
			}
			else if (comp_node["SpotLight"])
			{
				ent->AddComponent<SpotLight>(renderer.CreateSpotLight(ent->ID));
				ent->GetComponent<SpotLight>()->OnLoad(comp_node["SpotLight"]);
			}
			else if (comp_node["CameraComponent"])
			{
				ent->AddComponent<CameraComponent>(renderer.m_cameras.AddComponent(ent->ID));
				ent->GetComponent<CameraComponent>()->OnLoad(comp_node["CameraComponent"]);
			}
			else if (comp_node["ReflectionProbe"])
			{
				ent->AddComponent<ReflectionProbe>(renderer.CreateReflectionProbe(ent->ID));
				ent->GetComponent<ReflectionProbe>()->OnLoad(comp_node["ReflectionProbe"]);
			}
			else if (comp_node["RigidBody"])
			{
				m_PhysicsWorld.AddRigidBody(ent);
				RigidBody* rb = ent->GetComponent<RigidBody>();
				rb->OnLoad(comp_node["RigidBody"]);

				if (rb->m_CollisionType == RCollisionShapeType::MESH_COLLIDER) 
				{
					if (ent->HasComponent<RendererComponent>()) {
						btTriangleMesh* m = m_PhysicsWorld.GetMeshCollider(ent->GetComponent<RendererComponent>()->mesh);
						ent->GetComponent<RigidBody>()->SetCollisionMesh(m, ent->GetComponent<RendererComponent>()->mesh->path, ent->GetComponent<RendererComponent>()->mesh->index);
					}
					/*Model* mdl = resManager.mMeshs.CreateModel(rb->mesh_path);
					int inddx = 0;
					if ((rb->mesh_index + 1) > mdl->meshes.size())
						inddx = rb->mesh_index;
					Mesh* amesh = &mdl->meshes[inddx];
					btTriangleMesh* m = m_PhysicsWorld.GetMeshCollider(amesh);
					if(m != nullptr)
						rb->SetCollisionMesh(m, amesh->path, rb->mesh_index);*/
				}
				ent->SetScale(ent->transform.GetLocalScale());
			}
			else if (comp_node["RAudioSource"])
			{
				RAudioSource* ras = m_Audio.AddComponent(ent->ID);
				ent->AddComponent<RAudioSource>(ras);
				ras->OnLoad(comp_node["RAudioSource"]);
				ras->mClip = m_Audio.LoadClip(ras->clip_path);
			}
			else if (comp_node["BillboardComponent"])
			{
				BillboardComponent* bb = renderer.CreateBillboard(ent->ID);
				ent->AddComponent<BillboardComponent>(bb);
				bb->OnLoad(comp_node["BillboardComponent"]);
			}
			else if (comp_node["GrassComponent"])
			{
				GrassComponent* rc = renderer.mGrass.AddComponent(ent->ID);
				rc->OnLoad(comp_node["GrassComponent"]);
				rc->mTexture = resManager.CreateTexture(rc->texPath, rc->texPath.c_str());
				ent->AddComponent<GrassComponent>(rc);
			}
			else if (comp_node["ParticleSystem"])
			{
				ParticleSystem* ps = renderer.mParticals.AddComponent(ent->ID);
				ent->AddComponent<ParticleSystem>(ps);
				ps->OnLoad(comp_node["ParticleSystem"]);
				ps->mTexture = resManager.CreateTexture(ps->tex_path, ps->tex_path.c_str());
				ps->TargetPos = ent->transform.GetWorldPosition();
			}
			else if (comp_node["RendererComponent"])
			{
				auto rc_node = comp_node["RendererComponent"];
				RendererComponent* rc = renderer.m_renderers.AddComponent(ent->ID);
				ent->AddComponent<RendererComponent>(rc);
				rc->OnLoad(rc_node);

				if (rc_node["HasMesh"].as<bool>())
				{
					std::string dir = rc_node["mesh_source"].as<std::string>();
					int indx = rc_node["mesh_index"].as<int>();
					rc->mesh = &resManager.mMeshs.CreateModel(dir)->meshes[indx];
				}

				std::string mat_dir = rc_node["mat_source"].as<std::string>();
				std::string mat_name = rc_node["mat_name"].as<std::string>();

				rc->material = renderer.LoadMaterial(mat_dir.c_str());
				if (!rc->material->isDefault)
					rc->material->path = mat_name;
			}
			if (comp_node["SkeletalMeshComponent"])
			{
				auto sk_node = comp_node["SkeletalMeshComponent"];
				SkeletalMeshComponent* rc = renderer.m_skMeshs.AddComponent(ent->ID);
				ent->AddComponent<SkeletalMeshComponent>(rc);
				rc->OnLoad(sk_node);

				if (sk_node["HasMesh"].as<bool>())
				{
					std::string dir = sk_node["mesh_source"].as<std::string>();
					rc->mesh = resManager.mMeshs.LoadSkeletalModel(dir);
				}
				std::string mat_dir = sk_node["mat_source"].as<std::string>();
				std::string mat_name = sk_node["mat_name"].as<std::string>();
				rc->material = renderer.LoadMaterial(mat_dir.c_str());
				if (!rc->material->isDefault)
					rc->material->path = mat_name;
			}
			if (comp_node["UIWidget"])
			{
				ent->AddComponent<UIWidget>(m_UI.uiWidgets.AddComponent(ent->ID));
				ent->GetComponent<UIWidget>()->OnLoadCustom<cereal::BinaryInputArchive, ResourcesManager>(comp_node["UIWidget"], resManager);
			}
		}

		// Serialize scripts
		size_t script_count = entNode["script_count"].as<unsigned int>();
		YAML::Node scripts_node = entNode["Scripts"];

		for (auto scr_node : scripts_node)
		{
			std::string className = scr_node["class_name"].as<std::string>();
			ScriptInstance* scr = m_ScriptManager.InstanceComponentClass(ent->ID, className.c_str());
			ent->AddScript(scr);
			ScriptSerializer::LoadScriptObject(scr_node, scr);
		}

		// Serialize children
		size_t child_count = entNode["ChildCount"].as<unsigned int>();
		size_t last_child_indx = indx + child_count;
		for (size_t i = indx; i < last_child_indx; i++)
		{
			Entity* child = mScene.AddEntity("");
			child->SetParent(ent);
			indx++;
			indx = LoadAndSpawnEntity(scnNode, scnNode[indx], child, indx, false, false);
		}
	}
	return indx;
}

void Rogy::ClearScene()
{
	renderer.postProc.Use = false;
	renderer.SceneName = "";
	mScene.Root.RemoveAllChilds(IsPlaying());
	renderer.RemoveAllLights();
	renderer.Clear_Lightmaps();
	m_ScriptManager.ClearInstances();
	m_PhysicsWorld.ClearRigidbodies();
}

void Rogy::SaveScene(const char* path, bool temp)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	renderer.SerializeSave(out);
	mScene.SaveBP(out);
	out << YAML::EndMap;

	RYAML::EndSave(out, path);
}

void Rogy::LoadScene(const char* path, bool temp)
{
	m_PhysicsWorld.SetGravity(m_PhysicsWorld.grv);
	ClearScene();

	if (!temp) {
		mScene.name = "";
		mScene.Root.name = "New Scene";
	}	

	std::ifstream stream(path);

	if (!stream.is_open())
	{
		std::cout << "Unable to load scene : " << path << std::endl;
		return;
	}

	std::stringstream strStream;
	strStream << stream.rdbuf();
	YAML::Node data;

	data = YAML::Load(strStream.str());
	
	if(!temp)
	{
		mScene.name = data["Scene"].as<std::string>();
		eraseSubStr(mScene.name, std::string("\\"));
		eraseAllSubStr(mScene.name, std::string(".rscn")); // remove extension
		renderer.SceneName = mScene.name;
		mScene.path = path;
	}
	mScene.Root.name = mScene.name;
	//LoadEntityForSpawn(ar, &mScene.Root, true);

	renderer.SerializeLoad(data);

	auto entities = data["Entities"];
	if (entities)
	{
		//for (auto entity : entities)
		for (size_t i = 0; i < entities.size(); i++)
		{
			YAML::Node& entity = entities[i];
			if (entity["Root"].as<bool>())
				continue;

			Entity* ent = mScene.AddEntity("");
			//unsigned int child_count = entity["ChildCount"].as<unsigned int>();

			i = LoadAndSpawnEntity(entities, entity, ent, i, false, false);
		}
	}

	// New Scene Loaded
	if(IsPlaying())
		m_ScriptManager.BeginGame();
}

Entity* Rogy::SpawnEntity(std::string& path, bool justCopyPast)
{
	std::ifstream stream(path);
	if(!stream.is_open())
	{
		std::cout << "Unable to load prefab : " << path << std::endl;
#ifdef EDITOR_MODE
		m_Debug.Error("Unable to load prefab : " + path);
#endif
		return nullptr;
	}

	std::stringstream strStream;
	strStream << stream.rdbuf();
	YAML::Node data;

	data = YAML::Load(strStream.str());

	auto entities = data["Entities"];
	Entity* ent = mScene.AddEntity("Entity (Load Failed)");

	for (size_t i = 0; i < entities.size(); i++)
	{
		YAML::Node& entity = entities[i];
		if (entity["Root"].as<bool>())
			continue;

		i = LoadAndSpawnEntity(entities, entity, ent, i, false, false);
	}

	if (!justCopyPast) {
		ent->is_prefab = true;
		ent->path = path;
	}

	if (IsPlaying())
		ent->StartScripts();

	return ent;
}

void LoadModelToEntity(Model* model, Entity* ent, RModelNode* node, SceneManager& scene)
{
	ent->name = node->name;
	//ent->mesh = &model->meshes[node->idx];
	for (size_t i = 0; i < node->children.size(); i++)
	{
		Entity* ent_c = scene.AddEntity("model");
		ent_c->SetParent(ent);
		LoadModelToEntity(model, ent_c, node->children[i], scene);
	}
}

Entity* Rogy::SpawnModel(std::string& path)
{
	Model* mdl = resManager.mMeshs.CreateModel(path);
	Entity* ent = nullptr;
	if (mdl != nullptr)
	{
		//Entity* Bent = mScene.AddEntity("Model");
		for (size_t i = 0; i < mdl->meshes.size(); i++)
		{
			Entity* ent = mScene.AddEntity(mdl->meshes[i].name);
			//ent->SetParent(Bent);
			RendererComponent* rc = renderer.m_renderers.AddComponent(ent->ID);
			ent->AddComponent<RendererComponent>(rc);
			rc->material = renderer.CreateMaterial("");
			rc->mesh = &mdl->meshes[i];

			glm::vec3 translation, rotation, scale;
			LMath::DecomposeTransform(rc->mesh->transformation, translation, rotation, scale);

			ent->SetTranslation(translation);
			ent->transform.SetAngels(rotation);
			ent->SetScale(scale);
		}
	}
	return ent;
}

void Rogy::SetEntitiesLightmapPaths(Entity* ent)
{
	if (ent->Static && ent->HasComponent<RendererComponent>())
	{
		ent->GetComponent<RendererComponent>()->lightmapPath = renderer.mlightmaps[lmSetIndex]->getTexPath();
		lmSetIndex++;
	}

	for (size_t i = 0; i < ent->Children.size(); i++)
	{
		if (!ent->Children[i]->Active)
			continue;

		SetEntitiesLightmapPaths(ent->Children[i]);
	}
}

void Rogy::Clear()
{
	mScene.Root.RemoveAllChilds();
	window.Clear();
	renderer.Clear();
	resManager.Clear();
	m_Audio.Clear();
}

SceneManager* Rogy::GetScene()
{
	return &mScene;
}

#ifdef EDITOR_MODE
void  Rogy::RenderGuizmos()
{
	// Render 3d icons
	if (!mScene.game_view) {
		glm::mat4 proj = renderer.MainCam.GetProjectionMatrix();
		glm::mat4 view = renderer.MainCam.GetViewMatrix();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		renderer.materials.BillboardShader.use();
		renderer.materials.BillboardShader.SetMat4("VP", proj * view);
		renderer.materials.BillboardShader.SetVec3("CameraUp_worldspace", renderer.MainCam.transform.up());
		renderer.materials.BillboardShader.SetVec3("CameraRight_worldspace", renderer.MainCam.transform.right());
		renderer.materials.BillboardShader.SetVec2("BillboardSize", glm::vec2(0.2f, 0.2f));
		renderer.materials.BillboardShader.setBool("use_tex_as_mask", false);
		glActiveTexture(GL_TEXTURE0);

		for (size_t i = 0; i < m_icons.size(); i++)
		{
			if (m_icons[i].type == EditorIcon::EI_PLight && !renderer.m_PointLights.empty())
			{
				m_icons[i].image->useTexture();
				for (size_t j = 0; j < renderer.m_PointLights.size(); j++)
				{
					PointLight* pl = renderer.m_PointLights[j];
					renderer.materials.BillboardShader.SetVec3("BillboardPos", pl->Position);
					renderer.materials.BillboardShader.SetVec3("Tex_color", pl->Color);
					renderQuad();
				}
			}
			else if (m_icons[i].type == EditorIcon::EI_SLight && !renderer.m_SpotLights.empty())
			{
				m_icons[i].image->useTexture();
				for (size_t j = 0; j < renderer.m_SpotLights.size(); j++)
				{
					SpotLight* pl = renderer.m_SpotLights[j];
					renderer.materials.BillboardShader.SetVec3("BillboardPos", pl->Position);
					renderer.materials.BillboardShader.SetVec3("Tex_color", pl->Color);
					renderQuad();
				}
			}
			else if (m_icons[i].type == EditorIcon::EI_DLight && renderer.m_DirectionalLight != nullptr)
			{
				m_icons[i].image->useTexture();
				renderer.materials.BillboardShader.SetVec3("BillboardPos", renderer.m_DirectionalLight->Position);
				renderer.materials.BillboardShader.SetVec3("Tex_color", renderer.m_DirectionalLight->Color);
				renderQuad();
			}
			/*else if (m_icons[i].type == EditorIcon::EI_Camera && !renderer.m_cameras.Empty())
			{
				m_icons[i].image->useTexture();
				for (size_t j = 0; j < renderer.m_cameras.Size(); j++)
				{
					CameraComponent* pl = renderer.m_cameras.components[j];
					renderer.materials.BillboardShader.SetVec3("BillboardPos", pl->position);
					renderer.materials.BillboardShader.SetVec3("Tex_color", glm::vec3(1.0f));
					renderQuad();
				}
			}*/
			else if (m_icons[i].type == EditorIcon::EI_Audio && !m_Audio.mSources.Empty())
			{
				m_icons[i].image->useTexture();
				for (size_t j = 0; j < m_Audio.mSources.Size(); j++)
				{
					RAudioSource* pl = m_Audio.mSources.components[j];
					renderer.materials.BillboardShader.SetVec3("BillboardPos", pl->pos);
					renderer.materials.BillboardShader.SetVec3("Tex_color", glm::vec3(1.0f));
					renderQuad();
				}
			}
		}
		glDisable(GL_BLEND);
	}
}
#endif

// ---------------------------------------------------------------
// ## Play mode
// ---------------------------------------------------------------
void Rogy::RenderUI()
{
	editor.SCR_height = SCR_height;
	editor.SCR_weight = SCR_weight;
#ifdef  EDITOR_MODE
	// Editor
	editor.start();
	if (!mScene.game_view) {
		ImGuizmo::BeginFrame();

		if (!editor.s_hierarchy.sel_entt.empty()) {
			Entity* ent = mScene.FindEntity(editor.s_hierarchy.sel_entt[0]);

			if (ent->is_Selected == true)
			{
				// Entity transform
				glm::mat4 transform = ent->transform.GetTransform();

				glm::vec3 oldScale = ent->transform.GetLocalScale();

				float bounds[] = { -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f };
				float boundsSnap[] = { editor.snaping, editor.snaping, editor.snaping };

				ImGuizmo::Manipulate(glm::value_ptr(renderer.MainCam.GetViewMatrix()), glm::value_ptr(renderer.MainCam.GetProjectionMatrix()),
					editor.mCurrentGizmoOperation, editor.mCurrentGizmoMode, glm::value_ptr(transform), nullptr, (float*)&glm::vec3(editor.snaping)
				, editor.BoundSizing ? bounds : NULL, (editor.snaping > 0.0f) ? boundsSnap : NULL);

				gizHovred = ImGuizmo::IsOver();

				if (ImGuizmo::IsUsing())
				{
					//ent->SetTranslation(glm::vec3(transform[3]));
					glm::vec3 translation, rotation, scale;
					float matrixTranslation[3], matrixRotation[3], matrixScale[3];

					ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), matrixTranslation, matrixRotation, matrixScale);

					translation = glm::vec3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
					rotation = glm::vec3(matrixRotation[0], matrixRotation[1], matrixRotation[2]);
					scale = glm::vec3(matrixScale[0], matrixScale[1], matrixScale[2]);

					ent->SetTranslation(translation);
					ent->SetRotation(rotation);
					ent->SetScale(scale);
				}
			}
		}
		ImGuiIO& io = ImGui::GetIO();
		ImGuiViewport *viewport = ImGui::GetMainViewport();
		ImGuizmo::SetRect(viewport->Pos.x + MainViewport.left_pos, viewport->Pos.y + 57, MainViewport.weight, MainViewport.height);
	}
#else
	m_UI.BeginFrame();
#endif //  EDITOR_MODE
}

// ---------------------------------------------------------------
// ## Save game info
// ---------------------------------------------------------------
void Rogy::SaveProjectSettings()
{
	m_PlayerPrefs.Save();

	mProjectSettings.CascadedShadowMapsResolution = renderer.m_ShadowMapper.TEXEL_SIZE;
	mProjectSettings.CascadesCount = renderer.m_ShadowMapper.SHADOW_MAP_CASCADE_COUNT;
	mProjectSettings.ShadowDistance = (float)renderer.m_ShadowMapper.Shadow_Distance;
	mProjectSettings.PointShadowResolution = renderer.m_PointShadowMapper.TEXEL_SIZE;
	mProjectSettings.SpotShadowsResolution = renderer.m_SpotShadowMapper.TEXEL_SIZE;
	mProjectSettings.CascadeSplits[0] = renderer.m_ShadowMapper.CascadeSplits[0];
	mProjectSettings.CascadeSplits[1] = renderer.m_ShadowMapper.CascadeSplits[1];
	mProjectSettings.CascadeSplits[2] = renderer.m_ShadowMapper.CascadeSplits[2];

	mProjectSettings.CaptureResulotion = renderer.GetCaptureResolution();
	mProjectSettings.ReflectionProbeLimit = renderer.ibl.ReflectionProbeLimit;
	mProjectSettings.useDepthPrePass = renderer.DepthPrePass;
	mProjectSettings.useInstancing = renderer.UseInstancing;
	mProjectSettings.useInstancingForShadows = renderer.UseInstancingForShadows;

	std::ofstream os(ProjectResourcesFolder + "\\ProjectSettings", std::ios::binary);
	cereal::BinaryOutputArchive ar(os);
	ar(mProjectSettings);
	os.close();

	os.open(ProjectResourcesFolder + "\\InputSettings", std::ios::binary);
	cereal::BinaryOutputArchive ar2(os);
	m_Input.serializeSave<cereal::BinaryOutputArchive>(ar2);
	os.close();

	if (!Quiting)
	{

	}
}
void Rogy::LoadProjectSettings()
{
	m_PlayerPrefs.Load();

	std::filebuf fb;
	if (fb.open(ProjectResourcesFolder + "\\ProjectSettings", std::ios::in))
	{
		std::istream is(&fb);
		cereal::BinaryInputArchive ar(is);
		ar(mProjectSettings);
		fb.close();
	}

	if (fb.open(ProjectResourcesFolder + "\\InputSettings", std::ios::in))
	{
		std::istream is(&fb);
		cereal::BinaryInputArchive ar(is);
		m_Input.serializeLoad<cereal::BinaryInputArchive>(ar);
		fb.close();
	}

	renderer.m_ShadowMapper.TEXEL_SIZE = mProjectSettings.CascadedShadowMapsResolution;
	renderer.m_ShadowMapper.SHADOW_MAP_CASCADE_COUNT = mProjectSettings.CascadesCount;
	renderer.m_ShadowMapper.Shadow_Distance = mProjectSettings.ShadowDistance;
	renderer.m_PointShadowMapper.TEXEL_SIZE = mProjectSettings.PointShadowResolution;
	renderer.m_SpotShadowMapper.TEXEL_SIZE = mProjectSettings.SpotShadowsResolution;
	renderer.m_ShadowMapper.CascadeSplits[0] = mProjectSettings.CascadeSplits[0];
	renderer.m_ShadowMapper.CascadeSplits[1] = mProjectSettings.CascadeSplits[1];
	renderer.m_ShadowMapper.CascadeSplits[2] = mProjectSettings.CascadeSplits[2];

	renderer.SetCaptureResolution(mProjectSettings.CaptureResulotion);
	renderer.ibl.ReflectionProbeLimit = mProjectSettings.ReflectionProbeLimit;
	renderer.UseInstancing = mProjectSettings.useInstancing;
	renderer.UseInstancingForShadows = mProjectSettings.useInstancingForShadows;
	renderer.DepthPrePass = mProjectSettings.useDepthPrePass;
	renderer.m_SpotShadowMapper.MaxShadowCount = mProjectSettings.SpotShadowsLimit;


	//if (renderer.m_ShadowMapper.SHADOW_MAP_CASCADE_COUNT > 3)
		//renderer.m_ShadowMapper.SHADOW_MAP_CASCADE_COUNT = 3;
}

// ---------------------------------------------------------------
// Lua Scripting
// ---------------------------------------------
void Rogy::RecompileScripts()
{
	/* Reload lua state */
	m_ScriptManager.ReloadLuaInterpreter();
	BindEngineForScript();

	/* Compile scripts in resources folder */
	m_ScriptManager.RecompileScripts(false);

	/* Send engine systems to scripting side */
	LuaRef Func = getGlobal(m_ScriptManager.L, "Prepare_ScriptSide");
	Func(&mScene, &m_Input, &m_PhysicsWorld, &m_Debug, &m_Audio);

	LuaRef Func2 = getGlobal(m_ScriptManager.L, "Prepare_ScriptSide2");
	Func2(&m_PlayerPrefs);

	/* Re instance objects */
	m_ScriptManager.ReInstanceObjects();
}

void Rogy::BindEngineForScript()
{
	/* Bind all the input keys to Lua in namespace "RKey" */
	RKey::BindKeyToNamespace(m_ScriptManager.L, "RKey");

	//EngineAPI_LUA::RegisterAPI<Rogy>(m_ScriptManager.L);
	luabridge::getGlobalNamespace(m_ScriptManager.L)
		//SceneManager
		.beginClass<SceneManager>("SceneManager")
		.addFunction("SpawnEntity", &SceneManager::SpawnEntity)
		.addFunction("LoadScene", &SceneManager::LoadScene)
		.addFunction("QuitGame", &SceneManager::QuitGame)
		.addProperty("IsPlaying", &SceneManager::is_playing)
		.addFunction("GetEntity", &SceneManager::FindEntity)
		.addFunction("DestroyEntity", &SceneManager::DestroyEntity)
		.addFunction("GetRoot", &SceneManager::GetRoot)
		.addFunction("FindEntityWithTag", &SceneManager::FindEntityWithTag)
		.addFunction("CreateEntity", &SceneManager::CreateEntity<Rogy>)
		.addFunction("Broadcast", &SceneManager::BroadcastScriptFunc)
		.endClass()

		//Debug
		.beginClass<DebugTool>("DebugTool")
		.addFunction("Log", &DebugTool::Log)
		.addFunction("Warning", &DebugTool::Warning)
		.addFunction("Error", &DebugTool::Error)
		.addFunction("Clear", &DebugTool::Clear)
		.endClass()

		//Asset class
		.beginClass<AssetPath>("AssetPath")
		.addConstructor<void(*) ()>()
		.addData("__type", &AssetPath::__type)
		.addFunction("Set", &AssetPath::SetPath)
		.addFunction("Get", &AssetPath::GetPath)
		.addFunction("GetType", &AssetPath::GetType)
		.endClass()

		.beginClass<AudioClip>("AudioClip")
		.addProperty("Path", &AudioClip::mPath)
		.endClass()

		.beginClass<AudioManager>("AudioManager")
		.addFunction("LoadClip", &AudioManager::LoadClip)
		.addFunction("Play2D", &AudioManager::Play2D)
		.addFunction("Play3D", &AudioManager::Play3D)
		.endClass()

		.beginClass<PlayerPrefs>("PlayerPrefs_")
		.addFunction("Save", &PlayerPrefs::Save)
		.addFunction("AddString", &PlayerPrefs::AddString)
		.addFunction("AddInt", &PlayerPrefs::AddInt)
		.addFunction("AddFloat", &PlayerPrefs::AddFloat)
		.addFunction("GetFloat", &PlayerPrefs::GetFloat)
		.addFunction("GetInt", &PlayerPrefs::GetInt)
		.addFunction("GetString", &PlayerPrefs::GetString)
		.endClass()
		
		.beginNamespace("Time")
		.addVariable("time", &this->time, false)
		.addVariable("deltaTime", &this->deltaTime, false)
		.addVariable("PhyDeltaTime", &this->PhyDeltaTime, false)
		.addVariable("frameRateLimit", &this->frameRateLimit)
		.addVariable("FPS", &this->FPS)
		.endNamespace()

		.beginNamespace("Mathf")
		.addFunction("Clamp", &LMath::Clamp)
		.addFunction("Abs", &LMath::Abs)
		.addFunction("Sin", &LMath::Sin)
		.addFunction("PI", &LMath::PI)
		.addFunction("Random", &Random::Range)
		.addFunction("Sqrt", &LMath::Sqrt)
		.addFunction("LerpV", &LMath::LerpV)
		.addFunction("Lerp", &LMath::Lerp)
		.endNamespace()

		// Vector3
		.beginClass<glm::vec3>("Vector3")
		.addConstructor<void(*) (float, float, float)>()
		.addProperty("x", &Vec3Helper::get<0>, &Vec3Helper::set<0>)
		.addProperty("y", &Vec3Helper::get<1>, &Vec3Helper::set<1>)
		.addProperty("z", &Vec3Helper::get<2>, &Vec3Helper::set<2>)
		.addFunction("__add", &Vec3Helper::ADD)
		.addFunction("__mul", &Vec3Helper::MUL)
		.addFunction("__div", &Vec3Helper::DIV)
		.addFunction("__sub", &Vec3Helper::SUB)
		.addFunction("__lt", &Vec3Helper::lessThen)
		.addFunction("__le", &Vec3Helper::LessOrEq)
		.addFunction("__eq", &Vec3Helper::Equale)
		.addFunction("GetType", &Vec3Helper::__gettype)
		.addStaticFunction("Down", &Vec3Helper::VecDown)
		.addStaticFunction("Left", &Vec3Helper::VecLeft)
		.addStaticFunction("Up", &Vec3Helper::VecUp)
		.addStaticFunction("Right", &Vec3Helper::VecRight)
		.addStaticFunction("Forward", &Vec3Helper::VecForward)
		.addStaticFunction("Vec3F", &Vec3Helper::Vec3F)
		.addStaticFunction("Lerp", &Vec3Helper::Lerp)
		.addStaticFunction("Distance", &Vec3Helper::Distance)
		.addStaticFunction("Normalize", &Vec3Helper::Normalize)
		.addStaticFunction("CrossProduct", &Vec3Helper::CrossProduct)
		.addStaticFunction("ProjectOnPlane", &Vec3Helper::ProjectOnPlane)
		.endClass()

		// Vector2
		.beginClass<glm::vec2>("Vector2")
		.addConstructor<void(*) (float, float)>()
		.addProperty("x", &Vec2Helper::get<0>, &Vec2Helper::set<0>)
		.addProperty("y", &Vec2Helper::get<1>, &Vec2Helper::set<1>)
		.addFunction("__add", &Vec2Helper::ADD)
		.addFunction("__mul", &Vec2Helper::MUL)
		.addFunction("__div", &Vec2Helper::DIV)
		.addFunction("__sub", &Vec2Helper::SUB)
		.addFunction("__lt", &Vec2Helper::lessThen)
		.addFunction("__le", &Vec2Helper::LessOrEq)
		.addFunction("__eq", &Vec2Helper::Equale)
		.addStaticFunction("Up", &Vec2Helper::VecUp)
		.addStaticFunction("Right", &Vec2Helper::VecRight)
		.addStaticFunction("Vec2F", &Vec2Helper::Vec2F)
		.addStaticFunction("Lerp", &Vec2Helper::Lerp)
		.endClass()

		// --------------------------------------------------------------------------
		// Binding components
		// --------------------------------------------------------------------------
		.beginClass<Transform>("Transform")
			.addFunction("GetPosition", &Transform::GetLocalPosition)
			.addFunction("SetPosition", &Transform::SetPosition)
			.addFunction("GetWorldPosition", &Transform::GetWorldPosition)
			.addFunction("SetWorldPosition", &Transform::SetWorldPosition)
			.addFunction("GetScale", &Transform::GetLocalScale)
			.addFunction("SetScale", &Transform::SetLocalScale)
			.addFunction("GetAngels", &Transform::GetEurlerAngels)
			.addFunction("SetAngels", &Transform::SetAngels)
			.addFunction("Forward", &Transform::forward)
			.addFunction("Right", &Transform::right)
			.addFunction("Up", &Transform::up)
			.addFunction("LookAt", &Transform::LookAt)
			.addFunction("Translate", &Transform::Translate)
		.endClass()

		// RigidBody
		.beginClass<RigidBody>("RigidBody")
			.addFunction("Move", &RigidBody::Move)
			.addFunction("GetVelocity", &RigidBody::GetVelocity)
			.addFunction("SetVelocity", &RigidBody::SetVelocity)
			.addFunction("SetAngels", &RigidBody::SetAngels)
			.addFunction("AddForce", &RigidBody::AddForce)
			.addFunction("AddCentralForce", &RigidBody::AddCentralForce)
			.addFunction("SetGravity", &RigidBody::SetGravity)
			.addFunction("SetGravityY", &RigidBody::SetGravityY)
			.addFunction("GetGravity", &RigidBody::GetGravity)
			.addFunction("GetScale", &RigidBody::GetScale)
			.addFunction("SetScale", &RigidBody::SetScale)
			.addFunction("SetFriction", &RigidBody::SetFriction)
			.addFunction("GetFriction", &RigidBody::GetFriction)
			.addFunction("SetBounciness", &RigidBody::SetBounciness)
			.addFunction("GetBounciness", &RigidBody::GetBounciness)
			.addFunction("Activate", &RigidBody::Activate)
			.addFunction("ApplyImpulse", &RigidBody::ApplyImpulse)
		.endClass()

		.beginClass<PointLight>("PointLight")
			.addProperty("Enabled", &PointLight::enabled)
			.addProperty("Intensity", &PointLight::Intensity)
			.addProperty("Color", &PointLight::Color)
			.addProperty("Raduis", &PointLight::Raduis)
			.addProperty("CastShadows", &PointLight::CastShadows)
			.addProperty("Bias", &PointLight::Bias)
		.endClass()

		.beginClass<SpotLight>("SpotLight")
			.addProperty("Enabled", &SpotLight::enabled)
			.addProperty("Intensity", &SpotLight::Intensity)
			.addProperty("Color", &SpotLight::Color)
			.addProperty("Raduis", &SpotLight::Raduis)
			.addProperty("CastShadows", &SpotLight::CastShadows)
			.addProperty("Bias", &SpotLight::Bias)
			.addProperty("OuterCutOff", &SpotLight::OuterCutOff)
			.addProperty("CutOff", &SpotLight::CutOff)
		.endClass()

		.beginClass<DirectionalLight>("DirectionalLight")
			.addProperty("Intensity", &DirectionalLight::Intensity)
			.addProperty("Color", &DirectionalLight::Color)
			.addProperty("CastShadows", &DirectionalLight::CastShadows)
			.addProperty("Soft", &DirectionalLight::Soft)
			.addProperty("Bias", &DirectionalLight::Bias)
		.endClass()

		.beginClass<CameraComponent>("CameraComponent")
			.addProperty("FOV", &CameraComponent::FOV)
			.addProperty("Primary", &CameraComponent::Primary)
			.addProperty("FarView", &CameraComponent::FarView)
			.addProperty("NearView", &CameraComponent::NearView)
		.endClass()

		.beginClass<BillboardComponent>("BillboardComponent")
			.addProperty("enabled", &BillboardComponent::enabled)
			.addProperty("depth_test", &BillboardComponent::depth_test)
			.addProperty("color", &BillboardComponent::color)
			//.addProperty("size", &BillboardComponent::size)
			.addProperty("tex_path", &BillboardComponent::tex_path)
			.addProperty("sun_source", &BillboardComponent::sun_source)
		.endClass()

		.beginClass<ReflectionProbe>("ReflectionProbe")
			.addProperty("BoxProjection", &ReflectionProbe::BoxProjection)
			.addProperty("Intensity", &ReflectionProbe::Intensity)
			.addProperty("static_only", &ReflectionProbe::static_only)
			.addProperty("Resolution", &ReflectionProbe::Resolution)
			.addFunction("BakeReflections", &ReflectionProbe::BakeReflections)
		.endClass()

		.beginClass<ParticleSystem>("ParticleSystem")
			.addProperty("StartSize", &ParticleSystem::StartSize)
			.addProperty("StartColor", &ParticleSystem::StartColor)
			.addProperty("StartSpeed", &ParticleSystem::StartSpeed)
			.addProperty("StartLifeTime", &ParticleSystem::StartLifeTime)
			.addProperty("FadeOut", &ParticleSystem::FadeOut)
			.addProperty("PlayOnStart", &ParticleSystem::PlayOnStart)
			.addProperty("AddSizeOverTime", &ParticleSystem::AddSizeOverTime)
			.addProperty("EmitteCount", &ParticleSystem::EmitteCount)
			.addFunction("GetMaxParticleCount", &ParticleSystem::GetMaxParticleCount)
			.addFunction("SetMaxParticleCount", &ParticleSystem::SetMaxParticleCount)
			.addFunction("Emitte", &ParticleSystem::Emitte)
			.addFunction("Stop", &ParticleSystem::Stop)
			.addProperty("GravityModifier", &ParticleSystem::GravityModifier)
			.addProperty("OneDirection", &ParticleSystem::OneDirection)
			.addProperty("Spread", &ParticleSystem::Spread)
			.addProperty("Looping", &ParticleSystem::Looping)
			.addProperty("Emitting", &ParticleSystem::Emitting)
		.endClass()
			
		.beginClass<RAudioSource>("RAudioSource")
			.addFunction("Play", &RAudioSource::Play)
			.addFunction("Stop", &RAudioSource::Stop)
			.addProperty("PlayOnStart", &RAudioSource::PlayOnStart)
			.addFunction("SetLooping", &RAudioSource::SetLooping)
			.addFunction("GetLooping", &RAudioSource::GetLooping)
			.addFunction("SetMaxDistance", &RAudioSource::SetMaxDistance)
			.addFunction("GetMaxDistance", &RAudioSource::GetMaxDistance)
			.addFunction("SetMinDistance", &RAudioSource::SetMinDistance)
			.addFunction("GetMinDistance", &RAudioSource::GetMinDistance)
			.addFunction("SetVolume", &RAudioSource::SetVolume)
			.addFunction("GetVolume", &RAudioSource::GetVolume)
		.endClass()

		.beginClass<UIWidgetImage>("UIWidgetImage")
		.addProperty("Enabled", &UIWidgetImage::Enabled)
		.addProperty("name", &UIWidgetImage::name)
		.addProperty("Alpha", &UIWidgetImage::Alpha)
		.endClass()

		.beginClass<UIWidgetText>("UIWidgetText")
		.addProperty("Enabled", &UIWidgetText::Enabled)
		.addProperty("name", &UIWidgetText::name)
		.addProperty("Position", &UIWidgetText::Position)
		.addProperty("text", &UIWidgetText::text)
		.endClass()

		.beginClass<UIWidgetButton>("UIWidgetButton")
		.addProperty("Enabled", &UIWidgetButton::Enabled)
		.addProperty("name", &UIWidgetButton::name)
		.endClass()

		.beginClass<UIWidgetProgressBar>("UIWidgetProgressBar")
		.addProperty("Enabled", &UIWidgetProgressBar::Enabled)
		.addProperty("name", &UIWidgetProgressBar::name)
		.addProperty("value", &UIWidgetProgressBar::Value)
		.endClass()

		.beginClass<UIWidget>("UIWidget")
		.addProperty("Alpha", &UIWidget::Alpha)
		.addProperty("Position", &UIWidget::Position)
		.addProperty("Enabled", &UIWidget::enabled)
		.addFunction("GetText", &UIWidget::GetWidgetWithName<UIWidgetText>)
		.addFunction("GetButton", &UIWidget::GetWidgetWithName<UIWidgetButton>)
		.addFunction("GetBar", &UIWidget::GetWidgetWithName<UIWidgetProgressBar>)
		.addFunction("GetImage", &UIWidget::GetWidgetWithName<UIWidgetImage>)
		.endClass()
		
		// --------------------------------------------------------------------------
		// End binding components
		// --------------------------------------------------------------------------

		// Entity
		.beginClass<Entity>("Entity")
		.addData("ID", &Entity::ID, false)
		.addProperty("Active", &Entity::Active)
		.addProperty("parent", &Entity::parent)
		.addProperty("name", &Entity::name)
		.addProperty("tag", &Entity::tag)
		.addProperty("transform", &Entity::transform)
		.addFunction("DestroySelf", &Entity::DestroySelf)
		.addFunction("DestroySelfIn", &Entity::DestroySelfIn)
		.addFunction("SetParent", &Entity::SetParent)
		.addFunction("GetChild", &Entity::GetChild)
		.addFunction("SetRotation", &Entity::SetRotation)
		.addFunction("SetTranslation", &Entity::SetTranslation)
		.addFunction("RotateY", &Entity::RotateY)
		.addFunction("Rotate", &Entity::Rotate)
		.addFunction("GetScriptIndex", &Entity::GetScriptInstance)
		.addFunction("GetChildWithTag", &Entity::GetChildWithTag)
		.addFunction("GetParentWithTag", &Entity::GetParentWithTag)
		.addFunction("Broadcast", &Entity::InvokeScriptFunc)
		.addProperty("ContactPos", &Entity::ContactPos)
		.addProperty("ContactNormal", &Entity::ContactNormal)

		.addFunction("GetParticleSystem", &Entity::GetComponent<ParticleSystem>)
		.addFunction("GetAudioSource", &Entity::GetComponent<RAudioSource>)
		.addFunction("GetRigidBody", &Entity::GetComponent<RigidBody>)
		.addFunction("GetPointLight", &Entity::GetComponent<PointLight>)
		.addFunction("GetSpotLight", &Entity::GetComponent<SpotLight>)
		.addFunction("GetDirectionalLight", &Entity::GetComponent<DirectionalLight>)
		.addFunction("GetCameraComponent", &Entity::GetComponent<CameraComponent>)
		.addFunction("GetBillboardComponent", &Entity::GetComponent<BillboardComponent>)
		.addFunction("GetReflectionProbe", &Entity::GetComponent<ReflectionProbe>)
		.addFunction("GetUIWidget", &Entity::GetComponent<UIWidget>)
		.endClass()

		// InputManager
		.beginClass<InputManager>("InputManager")
		.addFunction("GetKey", &InputManager::GetKey)
		.addFunction("GetKeyDown", &InputManager::GetKeyDown)
		.addFunction("GetKeyUp", &InputManager::GetKeyUp)
		.addFunction("GetAxis", &InputManager::GetAxis)
		.addFunction("GetMouseXPos", &InputManager::GetMouseXPos)
		.addFunction("GetMouseYPos", &InputManager::GetMouseYPos)
		.addFunction("GetMouseXDelta", &InputManager::GetMouseXDelta)
		.addFunction("GetMouseYDelta", &InputManager::GetMouseYDelta)
		.addFunction("GetMouseButton", &InputManager::GetMouseButton)
		.addFunction("GetMouseButtonDown", &InputManager::GetMouseButtonDown)
		.addFunction("GetMouseButtonUp", &InputManager::GetMouseButtonUp)
		.addFunction("SetCursor", &InputManager::SetCursor)
		.endClass()

		// Physics
		.beginClass<RayHitInfo>("RayHitInfo")
		.addConstructor<void(*) ()>()
		.addProperty("hasHit", &RayHitInfo::hasHit)
		.addProperty("distance", &RayHitInfo::distance)
		.addProperty("body", &RayHitInfo::body)
		.addProperty("point", &RayHitInfo::point)
		.addProperty("normal", &RayHitInfo::normal)
		.endClass()

		.beginClass<PhysicsWorld>("PhysicsWorld")
		.addFunction("Raycast", &PhysicsWorld::Raycast)
		.addFunction("RayTest", &PhysicsWorld::RayTest)
		.addFunction("CheckSphere", &PhysicsWorld::CheckSphere)
		.addFunction("GetGravity", &PhysicsWorld::GetGravity)
		.addFunction("SetGravity", &PhysicsWorld::SetGravity)
		.endClass();
}