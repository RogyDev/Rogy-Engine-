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
	window.SetWindowTitle("Rogy Editor");
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
	frameRateLimit = 60.0f;

	// Initialize Render Engine
	// ----------------------------
	renderer.Initialize(SCR_weight, SCR_height, window.window, &resManager);

	return true;
}

bool Rogy::Init()
{
	RGetCurrentPath();
	Rogy::getIns(this);

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

	// Initialize Scripting
	// ---------------------------
	m_ScriptManager.debug = &m_Debug;
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
			editor.prj_browser.mScene = &mScene;
			editor.prj_browser.rnder = &renderer;
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
	/*YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << "Untitled";
	out << YAML::Key << "Test2" << YAML::Value << "aaa";
	out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
	out << YAML::Key << "Test" << YAML::Value << 55;
	out << YAML::EndSeq;
	out << YAML::EndMap;
	std::ofstream fout("test.yaml");
	fout << out.c_str();*/

	/*std::ifstream stream("test.yaml");
	std::stringstream strStream;
	strStream << stream.rdbuf();

	YAML::Node data = YAML::Load(strStream.str());
	if (data["Scene"])
		std::cout << "DATA : " << data["Scene"].as<std::string>() << std::endl;*/

	/* ----------------------------------------------
	 Set a unique id for each component by registering it
	 Warning : Shouldn't reordered; because components types are
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

void Rogy::MainLoop()
{
	if (!Init_succes) return;

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
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		FPS = (int)(1.0f / deltaTime);
		renderer.fps = FPS;

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
		/* Update scene entities */
		mScene.Root.Update(deltaTime);
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
			if (EditorMode) {
				if (mScene.show_grid)
					dd::xzSquareGrid(-50.0f, 50.0f, -1.0f, 1.0f, dd::colors::Gray);
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
#endif
		renderer.EndFrame();
		
		if(mScene.game_view)
			m_UI.RenderBlurFrames(renderer.postProc.BluredScreen.textureColorbuffer);

		// Render UI
		// ----------------------------------------
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

	bool active = ent->Active;
	for (size_t i = 0; i < ent->m_components.size(); i++)
	{
		// Push Renderer component to the renderer
		if (Component::IsComponentType<RendererComponent>(ent->m_components[i]))
		{
			RendererComponent* rc = Component::QuickCast<RendererComponent>(ent->m_components[i]);
			if (rc->mesh) {
				rc->enabled = ent->Active;
				rc->IsStatic = ent->Static;

				if (ChangedTransform) {
					rc->position = transform.GetWorldPosition();
					rc->transform = transform.GetTransform();
					rc->bbox = rc->mesh->bbox;
					rc->bbox.Transform(rc->transform, rc->position);
					//rc->bbox.Transforme(rc->transform);
					/*glm::vec3 ws = transform.GetWorldScale();
					rc->bbox.BoxMax = (rc->mesh->bbox.BoxMax * ws) + transform.GetWorldPosition();
					rc->bbox.BoxMin = (rc->mesh->bbox.BoxMin * ws) + transform.GetWorldPosition();
					rc->bbox.radius = (ws.x + ws.y + ws.z) / 3;
					/*rc->bbox.radius = rc->mesh->bbox.radius + ws.x - 1;
					rc->bbox.radius = rc->bbox.radius + ws.y - 1;
					rc->bbox.radius = rc->bbox.radius + ws.z - 1;
					if (transform.Angels != glm::vec3(0.0f, 0.0f, 0.0f))
						rc->bbox.useRaduis = true;
					else rc->bbox.useRaduis = false;*/
				}

				/*if (ent->ID == wpn->ID)
				{
					SkeletalMeshComponent* skmc = anim->GetComponent<SkeletalMeshComponent>();
					rc->transform = skmc->animator.wp;
					rc->position = glm::vec3(rc->transform[3]);
					/*bool wp_node_found = false;
					std::cout << "--------------------------------------------------------\n";
					glm::mat4 mt = skmc->animator.GetNodeTransform(skmc->animator.GetAnimationRootNode(),
						std::string("wp"), wp_node_found);
					std::cout << "--------------------------------------------------------\n";
					if (wp_node_found)
					{
						//m_Debug.Log("wp node found");
						rc->transform = mt;
						rc->position = glm::vec3(mt[3]);
					}
				}*/

				if (!mScene.game_view && ent->is_Selected && !rc->bbox.useRaduis) {
					// Axis-aligned bounding box:
					const ddVec3 bbMins = { rc->bbox.BoxMin.x, rc->bbox.BoxMin.y, rc->bbox.BoxMin.z };
					const ddVec3 bbMaxs = { rc->bbox.BoxMax.x, rc->bbox.BoxMax.y, rc->bbox.BoxMax.z };
					const ddVec3 bbCenter = {(bbMins[0] + bbMaxs[0]) * 0.5f,(bbMins[1] + bbMaxs[1]) * 0.5f,(bbMins[2] + bbMaxs[2]) * 0.5f};
					dd::aabb(bbMins, bbMaxs, dd::colors::Gray);
				}
				
				renderer.PushRender(rc->mesh, rc->material, rc->transform, rc->bbox, rc->CastShadows, rc->position, rc->IsStatic, rc->lightmapPath);
			}
			continue;
		}

		if (Component::IsComponentType<RigidBody>(ent->m_components[i]))
		{
			RigidBody* rb = Component::QuickCast<RigidBody>(ent->m_components[i]);
			
			if (transform.m_Last_Transform != transform.m_Transform)
				rb->rigidBody->activate();

			btTransform& tr = rb->GetTransform();
			btVector3& ve = tr.getOrigin();
			btQuaternion& qu = tr.getRotation();
			quat q = quat(qu.getW(), qu.getX(), qu.getY(), qu.getZ());
			transform.SetRotation(q);
			//transform.SetPosition(ve.getX(), ve.getY(), ve.getZ());
			transform.SetPosition(glm::vec3(ve.getX(), ve.getY(), ve.getZ()) + rb->mOffset);

			if(ent->is_Selected && !mScene.game_view)
				m_PhysicsWorld.dynamicsWorld->debugDrawObject(rb->rigidBody->getWorldTransform(), rb->rigidBody->getCollisionShape(), btVector3(1, 1, 1));
			continue;
		}

		if (Component::IsComponentType<PointLight>(ent->m_components[i]))
		{
			PointLight* point_light = Component::QuickCast<PointLight>(ent->m_components[i]);
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

		if (Component::IsComponentType<DirectionalLight>(ent->m_components[i]))
		{
			DirectionalLight* dir_light = Component::QuickCast<DirectionalLight>(ent->m_components[i]);
			dir_light->Active = active;
			dir_light->Direction = transform.forward();
			continue;
		}

		if (Component::IsComponentType<SpotLight>(ent->m_components[i]))
		{
			SpotLight* spot_light = Component::QuickCast<SpotLight>(ent->m_components[i]);
			spot_light->Active = active;
			spot_light->Static = ent->Static;
			spot_light->Direction = transform.forward();
			spot_light->Position = transform.GetWorldPosition();
			continue;
		}
		if (Component::IsComponentType<ReflectionProbe>(ent->m_components[i]))
		{
			ReflectionProbe* ref_probe = Component::QuickCast<ReflectionProbe>(ent->m_components[i]);
			ref_probe->Active = active;
			ref_probe->Position = transform.GetWorldPosition();

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
			}
			continue;
		}

		if (Component::IsComponentType<BillboardComponent>(ent->m_components[i]))
		{
			BillboardComponent* bb = Component::QuickCast<BillboardComponent>(ent->m_components[i]);
			bb->pos = transform.GetWorldPosition();
			bb->dir = transform.forward();
			continue;
		}
		if (Component::IsComponentType<ParticleSystem>(ent->m_components[i]))
		{
			ParticleSystem* bb = Component::QuickCast<ParticleSystem>(ent->m_components[i]);
			bb->TargetPos = transform.GetWorldPosition();
			bb->Direction = transform.forward();
			continue;
		}
		// Update Camera info
		if (Component::IsComponentType<CameraComponent>(ent->m_components[i]))
		{
			CameraComponent* cam = Component::QuickCast<CameraComponent>(ent->m_components[i]);
			cam->position = transform.GetWorldPosition();
			cam->right = transform.right();
			cam->direction = transform.forward();
			cam->up = transform.up();
			continue;
		}
		if (Component::IsComponentType<RAudioSource>(ent->m_components[i]))
		{
			RAudioSource* audio = Component::QuickCast<RAudioSource>(ent->m_components[i]);
			audio->SetPosition(transform.GetWorldPosition());
			continue;
		}
		if (Component::IsComponentType<SkeletalMeshComponent>(ent->m_components[i]))
		{
			SkeletalMeshComponent* anim = Component::QuickCast<SkeletalMeshComponent>(ent->m_components[i]);
			anim->position = ent->transform.GetWorldPosition();
			anim->transform = transform.GetTransform();
			continue;
		}
		if (Component::IsComponentType<NativeScriptComponent>(ent->m_components[i]))
		{
			NativeScriptComponent* nsc = Component::QuickCast<NativeScriptComponent>(ent->m_components[i]);
			if (!nsc->Instance)
			{
				nsc->Instance = nsc->InstantiateScript();
				nsc->Instance->m_entity = ent;
				nsc->Instance->OnCreate();
			}

			nsc->Instance->OnUpdate(deltaTime);
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
		if (mScene.m_requests[i] == SR_PAST_COPY)
		{
			SpawnEntity(std::string("savedCopy"), true);
			break;
		}
		if (mScene.m_requests[i] == SR_PLAY_SCENE)
		{
			if (IsPlaying())
				StopPlay();
			else
				BeginPlay();
			break;
		}
		if (mScene.m_requests[i] == SR_QUIT_GAME)
		{
			window.CloseWindow();
			break;
		}
		if (mScene.m_requests[i] == SR_NEW_SCENE)
		{
			ClearScene();
			mScene.name = "";
			mScene.path = "";
			mScene.Root.name = "New Scene";
			break;
		}
		if (mScene.m_requests[i] == SR_LOAD_SCENE)
		{
			if(!IsPlaying())
				m_Debug.Log(("Loading Scene: " + mScene.path));
			LoadScene(mScene.path.c_str());
			break;
		}
		if (mScene.m_requests[i] == SR_SAVE_SCENE)
		{
			SaveScene(mScene.path.c_str());
			if (!IsPlaying())
				m_Debug.Log(("Scene Saved : " + mScene.path));
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
/*
void Rogy::LoadEntityForSpawn(cereal::BinaryInputArchive &ar, Entity* ent, bool is_load_scene_root, bool LoadCustomTransformation)
{
	ar(ent->is_prefab);
	// If this entity is prefab then load it from its file
	// Prefab is minimized version of scene and can be spawned inside a scene.
	if (ent->is_prefab)
	{
		ar(ent->name);
		//ar(ent->transform);
		ar(ent->path);
		//std::cout << "LOADING A PREFAB IN SCENE : " << ent->path << std::endl;
		std::filebuf fb;
		if (fb.open(ent->path, std::ios::in))
		{
			std::istream is(&fb);
			cereal::BinaryInputArchive ar(is);

			LoadEntityForSpawn(ar, ent, false, true);
			fb.close();
		}
	}
	else
	{
		std::string ent_name;
		ar(ent_name);
		if (LoadCustomTransformation)
		{
			// Do not apply the loaded transformation from the prefab when loading
			// Just use the last applied trasformation
			ent->transform.noApply = true;
		} else  ent->name = ent_name;

		ar(ent->tag);
		ar(ent->Active);
		ar(ent->Static);
		//ar(ent->transform);

		// Load entity's components one by one
		int num_comp = 0; ar(num_comp);
		int comp_id = -1;
		for (int i = 0; i < num_comp; i++)
		{
			ar(comp_id);

			if (comp_id == RendererComponent::TYPE_ID)
			{
				RendererComponent* rc = renderer.m_renderers.AddComponent(ent->ID);
				ent->AddComponent<RendererComponent>(rc);
				ar(rc->enabled);
				ar(rc->CastShadows);
				ar(rc->lightmapPath);
				bool hasMesh = false;
				ar(hasMesh);
				if (hasMesh)
				{
					std::string dir;
					int indx = 0;
					ar(dir);
					ar(indx);
					rc->mesh = &resManager.mMeshs.CreateModel(dir)->meshes[indx];
				}

				std::string mat_dir, mat_name;
				ar(mat_dir);
				ar(mat_name);
				rc->material = renderer.LoadMaterial(mat_dir.c_str());
				if (!rc->material->isDefault)
					rc->material->path = mat_name;
			}
			else if (comp_id == PointLight::TYPE_ID)
			{
				ent->AddComponent<PointLight>(renderer.CreatePointLight(ent->ID));
				ent->GetComponent<PointLight>()->SerializeLoad<cereal::BinaryInputArchive>(ar);
			}
			else if (comp_id == SpotLight::TYPE_ID)
			{
				ent->AddComponent<SpotLight>(renderer.CreateSpotLight(ent->ID));
				ent->GetComponent<SpotLight>()->SerializeLoad<cereal::BinaryInputArchive>(ar);
			}
			else if (comp_id == DirectionalLight::TYPE_ID) {
				ent->AddComponent<DirectionalLight>(renderer.CreateDirectionalLight());
				ent->GetComponent<DirectionalLight>()->SerializeLoad<cereal::BinaryInputArchive>(ar);
			}
			else if (comp_id == ReflectionProbe::TYPE_ID)
			{
				ent->AddComponent<ReflectionProbe>(renderer.CreateReflectionProbe(ent->ID));
				ent->GetComponent<ReflectionProbe>()->SerializeSave<cereal::BinaryInputArchive>(ar);
			}
			else if (comp_id == RigidBody::TYPE_ID)
			{
				m_PhysicsWorld.AddRigidBody(ent);
				ent->GetComponent<RigidBody>()->SerializeLoad<cereal::BinaryInputArchive>(ar);
				ent->SetScale(ent->transform.GetLocalScale());
			}
			else if (comp_id == BillboardComponent::TYPE_ID)
			{
				BillboardComponent* bb = renderer.CreateBillboard(ent->ID);
				ent->AddComponent<BillboardComponent>(bb);
				bb->SerializeLoad<cereal::BinaryInputArchive>(ar);
			}
			else if (comp_id == CameraComponent::TYPE_ID)
			{
				ent->AddComponent<CameraComponent>(renderer.m_cameras.AddComponent(ent->ID));
				ent->GetComponent<CameraComponent>()->SerializeLoad<cereal::BinaryInputArchive>(ar);
			}
			else if (comp_id == ParticleSystem::TYPE_ID)
			{
				ParticleSystem* ps = renderer.mParticals.AddComponent(ent->ID);
				ent->AddComponent<ParticleSystem>(ps);
				ps->serializeLoad<cereal::BinaryInputArchive>(ar);
				ps->mTexture = resManager.CreateTexture(ps->tex_path, ps->tex_path.c_str());
			}
			else if (comp_id == RAudioSource::TYPE_ID)
			{
				RAudioSource* ras = m_Audio.AddComponent(ent->ID);
				ent->AddComponent<RAudioSource>(ras);
				ras->SerializeLoad<cereal::BinaryInputArchive>(ar);
				ras->mClip = m_Audio.LoadClip(ras->clip_path);
			}
			else if (comp_id == GrassComponent::TYPE_ID)
			{
				GrassComponent* rc = renderer.mGrass.AddComponent(ent->ID);
				rc->SerializeLoad<cereal::BinaryInputArchive>(ar);
				rc->mTexture = resManager.CreateTexture(rc->texPath, rc->texPath.c_str());
				ent->AddComponent<GrassComponent>(rc);
			}
			else if (comp_id == SkeletalMeshComponent::TYPE_ID)
			{
				SkeletalMeshComponent* rc = renderer.m_skMeshs.AddComponent(ent->ID);
				ent->AddComponent<SkeletalMeshComponent>(rc);
				ar(rc->enabled);
				ar(rc->CastShadows);
				ar(rc->scale);
				bool hasMesh = false;
				ar(hasMesh);
				if (hasMesh)
				{
					std::string dir;
					int indx = 0;
					ar(dir);
					//ar(indx);
					rc->mesh = resManager.mMeshs.LoadSkeletalModel(dir);
				}

				std::string mat_dir, mat_name;
				ar(mat_dir);
				ar(mat_name);
				rc->material = renderer.LoadMaterial(mat_dir.c_str());
				if (!rc->material->isDefault)
					rc->material->path = mat_name;
				size_t anim_size;
				ar(anim_size);
				std::string anim_name, anim_path;
				for (size_t i = 0; i < anim_size; i++)
				{
					ar(anim_name);
					ar(anim_path);
					rc->LoadAnimation(anim_name, anim_path);
				}
			}
			else if (comp_id == UIWidget::TYPE_ID)
			{
				ent->AddComponent<UIWidget>(m_UI.uiWidgets.AddComponent(ent->ID));
				ent->GetComponent<UIWidget>()->SerializeLoad<cereal::BinaryInputArchive, ResourcesManager>(ar, resManager);
			}
		}
		size_t script_count; ar(script_count);
		for (size_t i = 0; i < script_count; i++)
		{
			//std::string className; ar(className);
			//ScriptInstance* scr = m_ScriptManager.InstanceComponentClass(ent->ID, className.c_str());
			//ent->AddScript(scr);
			//ScriptSerializer::LoadScriptObject(ar, scr);
		}
		
		int child_count; ar(child_count);
		for (int i = 0; i < child_count; i++)
		{
			Entity* child_ent = mScene.AddEntity("Entity (Load Failed)");
			child_ent->SetParent(ent);
			LoadEntityForSpawn(ar, child_ent, false);
		}
	}
}
*/

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
				ent->GetComponent<RigidBody>()->OnLoad(comp_node["RigidBody"]);
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
		Entity* ent = mScene.AddEntity("Model");
		RendererComponent* rc = renderer.m_renderers.AddComponent(ent->ID);
		ent->AddComponent<RendererComponent>(rc);
		rc->material = renderer.CreateMaterial("");
		rc->mesh = mdl->GetFirstMesh();
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

// ---------------------------------------------------------------
// ## Play mode
// ---------------------------------------------------------------
void Rogy::RenderUI()
{
	editor.SCR_height = SCR_height;
	editor.SCR_weight = SCR_weight;
#ifdef  EDITOR_MODE
	editor.start();
	if (!mScene.game_view) {
		ImGuizmo::BeginFrame();

		if (!editor.s_hierarchy.sel_entt.empty()) {
			Entity* ent = mScene.FindEntity(editor.s_hierarchy.sel_entt[0]);

			if (ent->is_Selected == true)
			{
				// Entity transform
				glm::mat4 transform = ent->transform.GetTransform();

				ImGuizmo::Manipulate(glm::value_ptr(renderer.MainCam.GetViewMatrix()), glm::value_ptr(renderer.MainCam.GetProjectionMatrix()),
					editor.mCurrentGizmoOperation, editor.mCurrentGizmoMode, glm::value_ptr(transform), nullptr, nullptr);

				// Translate only -- other things were buggy
				if (ImGuizmo::IsUsing())
					ent->SetTranslation(glm::vec3(transform[3]));
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

	std::ofstream os("core\\ProjectSettings", std::ios::binary);
	cereal::BinaryOutputArchive ar(os);
	ar(mProjectSettings);
	os.close();

	os.open("core\\InputSettings", std::ios::binary);
	cereal::BinaryOutputArchive ar2(os);
	m_Input.serializeSave<cereal::BinaryOutputArchive>(ar2);
	os.close();
}
void Rogy::LoadProjectSettings()
{
	m_PlayerPrefs.Load();

	std::filebuf fb;
	if (fb.open("core\\ProjectSettings", std::ios::in))
	{
		std::istream is(&fb);
		cereal::BinaryInputArchive ar(is);
		ar(mProjectSettings);
		fb.close();
	}

	if (fb.open("core\\InputSettings", std::ios::in))
	{
		std::istream is(&fb);
		cereal::BinaryInputArchive ar(is);
		m_Input.serializeLoad<cereal::BinaryInputArchive>(ar);
		fb.close();
	}

	renderer.m_ShadowMapper.TEXEL_SIZE = mProjectSettings.CascadedShadowMapsResolution;
	renderer.m_ShadowMapper.SHADOW_MAP_CASCADE_COUNT = mProjectSettings.CascadesCount;
	renderer.m_ShadowMapper.Shadow_Distance = (size_t)mProjectSettings.ShadowDistance;
	renderer.m_PointShadowMapper.TEXEL_SIZE = mProjectSettings.PointShadowResolution;
	renderer.m_SpotShadowMapper.TEXEL_SIZE = mProjectSettings.SpotShadowsResolution;
	renderer.m_ShadowMapper.CascadeSplits[0] = mProjectSettings.CascadeSplits[0];
	renderer.m_ShadowMapper.CascadeSplits[1] = mProjectSettings.CascadeSplits[1];
	renderer.m_ShadowMapper.CascadeSplits[2] = mProjectSettings.CascadeSplits[2];
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
		
		.beginNamespace("Mathf")
		.addFunction("Clamp", &LMath::Clamp)
		.addFunction("Abs", &LMath::Abs)
		.addFunction("Sin", &LMath::Sin)
		.addFunction("PI", &LMath::PI)
		.addFunction("Random", &Random::Range)
		.addFunction("Sqrt", &LMath::Sqrt)
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
		.addStaticFunction("Up", &Vec3Helper::VecUp)
		.addStaticFunction("Right", &Vec3Helper::VecRight)
		.addStaticFunction("Forward", &Vec3Helper::VecForward)
		.addStaticFunction("Vec3F", &Vec3Helper::Vec3F)
		.addStaticFunction("Lerp", &Vec3Helper::Lerp)
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
			.addFunction("SetScale", &Transform::GetLocalScale)
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
		.endClass()

		.beginClass<PointLight>("PointLight")
			.addProperty("Intensity", &PointLight::Intensity)
			.addProperty("Color", &PointLight::Color)
			.addProperty("Raduis", &PointLight::Raduis)
			.addProperty("CastShadows", &PointLight::CastShadows)
			.addProperty("Bias", &PointLight::Bias)
		.endClass()

		.beginClass<SpotLight>("SpotLight")
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

		.beginClass<UIWidgetText>("UIWidgetText")
		.addProperty("Enabled", &UIWidgetText::Enabled)
		.addProperty("name", &UIWidgetText::name)
		.addProperty("text", &UIWidgetText::text)
		.endClass()

		.beginClass<UIWidgetButton>("UIWidgetButton")
		.addProperty("Enabled", &UIWidgetButton::Enabled)
		.addProperty("name", &UIWidgetButton::name)
		.endClass()

		.beginClass<UIWidget>("UIWidget")
		.addFunction("GetText", &UIWidget::GetWidgetWithName<UIWidgetText>)
		.addFunction("GetButton", &UIWidget::GetWidgetWithName<UIWidgetButton>)
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
		.addFunction("GetScriptIndex", &Entity::GetScriptInstance)
		.addFunction("GetChildWithTag", &Entity::GetChildWithTag)

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
		.addFunction("GetGravity", &PhysicsWorld::GetGravity)
		.addFunction("SetGravity", &PhysicsWorld::SetGravity)
		.endClass();
}