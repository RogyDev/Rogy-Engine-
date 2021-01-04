#ifndef EDITOR_PROPERTIES_H
#define EDITOR_PROPERTIES_H

#include "../resources/resources_manger.h"
#include "../renderer/renderer.h"
#include "../physics/PhysicsWorld.h"
#include "../audio/AudioManager.h"

#include "scene_hierarchy.h"

#define WIN32

class EditorProperty
{
public:
	EditorProperty();
	~EditorProperty();

	ResourcesManager* res = nullptr;
	Scene_hierarchy* nodes = nullptr;
	Renderer* rndr = nullptr;
	PhysicsWorld* phy_world = nullptr;
	ScriptManager* scrMnger = nullptr;
	AudioManager* audio_mnger = nullptr;

	bool showMat = false;

	static void BeginPreps(const char* nm);
	static void PrepName(const char* p_name);
	static void NextPreps();
	static void EndPreps();
	static std::string GetRelativePath(std::string target);
	static void SetWeightPrep();

	void EditTextureProprty(Material* tex);

	void point_light_editor(Entity& obj);

	void billboard_editor(Entity& obj);

	void general_editor(Entity &obj);
	void mesh_editor(Entity &obj);
	void material_editor(RendererComponent* rc);
	
	void spot_light_editor(Entity &obj);
	void dir_light_editor(Entity &obj);
	void ref_probe_editor(Entity &obj);
	void rb_editor(Entity &obj);
	void scriptC_editor(Entity &obj, ScriptInstance* script);
	void particles_editor(Entity &obj);
	void audio_editor(Entity &obj);
	void grass_editor(Entity &obj);
	
	void cam_editor(Entity &obj);

	void components_editor(Entity &obj);

	void CreateEntityWithMesh(std::string& ent_name, std::string& mesh_path);

	void Render();
private:
	EnttID save_ent_id = -1;

	template<typename T, typename UIFunction>
	bool DrawComponent(const std::string& name, Entity* entity, UIFunction uiFunction);
};

#endif