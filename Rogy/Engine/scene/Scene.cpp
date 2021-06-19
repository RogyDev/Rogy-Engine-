#include "Scene.h"
// ---------------------------------------------------------------
SceneManager::SceneManager()
{
	name = "Scene";

	CounterID = 0;
	Root.ID = -1;
	Root.name = "Root";
}
// ---------------------------------------------------------------

SceneManager::~SceneManager()
{
	std::cout << "SCENE INSTANCE DESTROYED";
}
// ---------------------------------------------------------------
// ---------------------------------------------------------------
void SceneManager::SetEntityParent(EnttID target, EnttID newParent)
{
	if (target == Root.ID) return; // cannot set scene root as a child

	Entity* tar = FindEntity(target);
	if (tar != nullptr)
	{
		Entity* par = FindEntity(newParent);
		if (par != nullptr)
		{
			if(par->parent != nullptr && par->parent->ID == target) return;
			if (tar->parent == par) return;
			tar->SetParent(par);
		}
	}
}
// ---------------------------------------------------------------
Entity* SceneManager::GetRoot()
{
	return &Root;
}
// ---------------------------------------------------------------
bool SceneManager::DestroyEntity(Entity* target)
{
	if (target != nullptr)
	{
		Entity* tgt = FindEntity(target->ID);
		if (tgt != nullptr) {
			tgt->DestroySelf();
			return true;
		}
	}
	return false;
}
// ---------------------------------------------------------------
Entity* SceneManager::FindEntity(EnttID entity_id)
{
	return Root.Find_entity(entity_id, &Root);
}
// ---------------------------------------------------------------
Entity * SceneManager::FindEntityWithTag(std::string ent_tag)
{
	return Root.GetChildWithTag(&Root, ent_tag);
}
// ---------------------------------------------------------------
void SceneManager::BroadcastScriptFunc(std::string func_name)
{
	Root.BroadcastFunc(&Root, func_name);
}
// ---------------------------------------------------------------
Entity* SceneManager::AddEntity(std::string ent_name)
{
	return AddEntity(-1, ent_name);
}
// ---------------------------------------------------------------
Entity* SceneManager::AddEntity(EnttID entity_id, std::string Name)
{
	Entity* ent = Root.Find_entity(entity_id, &Root);
	Entity* ret = nullptr;

	if (ent != nullptr && ent->ID == entity_id)
	{
		m_entities.push_back(CounterID);
		ret = ent->AddChild(Name, CounterID);
		CounterID++;
	}
	return ret;
}
// ---------------------------------------------------------------
void SceneManager::RemoveEntityID(EnttID eid)
{
	for (size_t i = 0; i < m_entities.size(); i++)
	{
		if (m_entities[i] == eid)
		{
			m_entities.erase(m_entities.begin() + i);
		}
	}
}
// ---------------------------------------------------------------
bool SceneManager::RemoveEntity(EnttID entity_id)
{
	Entity* ent = Root.Find_entity(entity_id, &Root);
	if (ent) {
		RemoveEntityIDFromList(ent);
		ent->DestroySelf();
	}

	return true;
}
// ---------------------------------------------------------------
void SceneManager::RemoveEntityIDFromList(Entity* ent)
{
	RemoveEntityID(ent->ID);

	for (size_t i = 0; i < ent->Children.size(); i++)
	{
		RemoveEntityIDFromList(ent->Children[i]);
	}
}
// ---------------------------------------------------------------
bool SceneManager::SaveEntityPrefab(EnttID id, const char* path)
{
	Entity* target = FindEntity(id);
	if (target)
	{
		target->SaveEntityFile(path);
		return true;
	}
	return false;
}
// ---------------------------------------------------------------
void SceneManager::PushRequest(SceneRequest req)
{
	m_requests.push_back(req);
}
// ---------------------------------------------------------------
void SceneManager::SaveScene(std::string pth)
{
	path = pth;
	PushRequest(SR_SAVE_SCENE);
}
// ---------------------------------------------------------------
void SceneManager::LoadScene(std::string pth)
{
	path = pth;
	PushRequest(SR_LOAD_SCENE);
}
// ---------------------------------------------------------------
void SceneManager::NewScene()
{
	PushRequest(SR_NEW_SCENE);
}
// ---------------------------------------------------------------
void SceneManager::QuitGame()
{
	PushRequest(SR_QUIT_GAME);
}
// ---------------------------------------------------------------
void SceneManager::SaveBP(YAML::Emitter& out)
{
	out << YAML::Key << "Scene" << YAML::Value << name;
	out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
	Root.SaveBP(out, true);
	out << YAML::EndSeq;
}
// ---------------------------------------------------------------
void SceneManager::SpawnEntity(std::string pth)
{
	spawn_requests.emplace_back(pth, false);
}
// ---------------------------------------------------------------
void SceneManager::SpawnModel(std::string& pth)
{
	spawn_requests.emplace_back(pth, true);
}
// ---------------------------------------------------------------
