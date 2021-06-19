#ifndef ROGY_ENGINE
#define ROGY_ENGINE

/* Comment this definition to disable the editor mode. */
#define EDITOR_MODE

#include <exception>

// include resources manager and scene manager
#include "resources\resources_manger.h"
#include "scene\Scene.h"

// Physics
#include "physics\PhysicsWorld.h"

// include the rendering engine
#include "renderer\renderer.h"

#include "shading\material.h"
#include "mesh\model.h"
#include "shading\viewportBounds.h"

// include editor
#include "editor\game_editor.h"
#include "ImGuizmo.h"

// Scripting
#include "scripting\ScriptManager.h"
#include "nativeScripting\ScriptableEntity.h"
#include "scripting\LMath.h"

// Input
#include "input\InputManager.h"

// Debug
#include "io/debug_tools.h"
#include "core\Vec3.h"

// Audio
#include "audio\AudioManager.h"

// UI
#include "ui\UIRenderer.h"

// include the window creator
#include "window\rogy_window.h"
#include <glm/gtc/type_ptr.hpp>

#include <RTools.h>
#include <thread>
#include "ProjectSettings.h"
#include "io/PlayerPrefs.h"

class Rogy
{
public:
	Rogy();
	~Rogy();
		
	const char* glsl_version;
	bool Init_succes = false;
	bool EditorMode = false;

	ViewPort_Bounds MainViewport;

	// timer
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	float frameRateLimit = 0.0f;
	int FPS = 0;

	//void Rogy::myTickCallback(btDynamicsWorld *world, btScalar timeStep);

	// PlayerPrefs - saves
	PlayerPrefs m_PlayerPrefs;

	// Project Settings
	RProjectSettings mProjectSettings;

	// The Rendering Engine
	Renderer renderer;

	// The resources manager.
	ResourcesManager resManager;

	// The main scene that we work with.
	SceneManager mScene;

	// The Physics World
	PhysicsWorld m_PhysicsWorld;

	// Scripting manager
	ScriptManager m_ScriptManager;

	// Input Manager
	InputManager m_Input;

	// Audio Manager
	AudioManager m_Audio;

	// Debug
	DebugTool m_Debug;

	// Game User-Interface
	UIRenderer m_UI;

	// The game editor user interface.
	Game_Editor editor;

	// window.
	RWindow window;

	// Initialize the engine.
	bool Init();

	// Start The main loop.
	void MainLoop();

	// StartUp.
	void StartUp();

	void UpdateEntity(Entity* enti);

	bool InitGraphics();

	void RenderUI();
	
	void GrassEdit();

	void Clear();

	void QueueSpawnList();
	//void LoadEntityForSpawn(cereal::BinaryInputArchive &ar, Entity* ent, bool is_load_scene_root, bool LoadCustomTransformation = false);
	size_t LoadAndSpawnEntity(YAML::Node &scnNode, YAML::Node &entNode, Entity* ent, size_t& indx, bool is_scene_root, bool UseCustomTransformation = false);

	bool IsPlaying();
	void BeginPlay();
	void StopPlay();

	SceneManager* GetScene();
	void ClearScene();
	void SaveScene(const char* path, bool temp = false);
	void LoadScene(const char* path, bool temp = false);
	Entity* SpawnEntity(string& path, bool justCopyPast = false);
	Entity* SpawnModel(string& path);

	void RecompileScripts();
	void BindEngineForScript(); 

	void SaveProjectSettings();
	void LoadProjectSettings();

	static Rogy* getIns(Rogy* ins = nullptr)
	{
		static Rogy* instance = nullptr;
		if (ins != nullptr)
			instance = ins;
		return instance;
	}

private:
	void PushAllRenders();
	bool isPlaying = false;
	bool lmBaked = false;
	void SetEntitiesLightmapPaths(Entity* ent);
	int lmSetIndex = 0;
	bool mainSceneLoaded = false;
};

#endif // ! ROGY_ENGINE