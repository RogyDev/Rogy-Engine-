#include <iostream>
#include "Entity.h"

// --------------------------------------------------------------------------------------------
Entity::Entity()
{
	name = "Entity";
	tag  = "default";
}
// --------------------------------------------------------------------------------------------
Entity::~Entity()
{
	//std::cout << "Entity [" << name << "] Destroyed. \n";
}
// --------------------------------------------------------------------------------------------
void Entity::OnCollision(bool enter, Entity* trgt, glm::vec3 contactPos, glm::vec3 contactNormal)
{
	IsColliding = enter;
	ContactPos = contactPos;
	ContactNormal = contactNormal;
	for (size_t i = 0; i < m_scripts.size(); i++)
	{
		if (!m_scripts[i]->hasOnCollision) continue;
		m_scripts[i]->PrepareMethod("OnCollision");
		lua_pushboolean(m_scripts[i]->L, enter);
		if(trgt != nullptr)
			lua_pushinteger(m_scripts[i]->L, trgt->ID);
		else
			lua_pushinteger(m_scripts[i]->L, -1);
		m_scripts[i]->CallMethod(2);
	}
}
// --------------------------------------------------------------------------------------------
void Entity::Start()
{
	started = true;
	transform.m_Dirty = true;
	transform.UpdateTransform();
}
// --------------------------------------------------------------------------------------------
void Entity::Update(float dt)
{
	if (willDestroy)
	{
		destroyIn -= dt;
		if (destroyIn <= 0)
		{
			DestroySelf();
			return;
		}
	}

	if (!started)
		Start();

	// Update Transformation
	// ----------------------------------
	transform.UpdateTransform();

	for (size_t i = 0; i < Children.size(); i++)
	{
		if (transform.m_Dirty)
		{
			Children[i]->transform.m_Dirty = true;
		}

		Children[i]->Update(dt);
	}
	transform.m_Dirty = false;
}
// --------------------------------------------------------------------------------------------
bool Entity::IsRoot()
{
	return root;
}
// --------------------------------------------------------------------------------------------
void Entity::SetTranslation(glm::vec3 pos, bool local)
{
	if (pos == transform.GetWorldPosition())
		return;

	RigidBody* rigidbody = GetComponent<RigidBody>();
	if (rigidbody != nullptr)
	{
		rigidbody->SetPosition(pos);
	}
	else
	{
		if(local)
			transform.SetPosition(pos);
		else
			transform.SetWorldPosition(pos);
	}
}
// --------------------------------------------------------------------------------------------
void Entity::SetRotation(glm::vec3 angles)
{
	if (angles == transform.GetEurlerAngels())
		return;

	RigidBody* rigidbody = GetComponent<RigidBody>();
	if (rigidbody)
		rigidbody->SetAngels(angles);

	transform.SetAngels(angles);
}
// --------------------------------------------------------------------------------------------
void Entity::SetScale(glm::vec3 newScale)
{
	RigidBody* rigidbody = GetComponent<RigidBody>();
	if (rigidbody) {
		transform.SetScale(newScale);
		rigidbody->SetCollisionScale(newScale);
	}
	else
		transform.SetScale(newScale);
}
// --------------------------------------------------------------------------------------------
void Entity::RotateY(float angle)
{
	angle = glm::radians(angle);
	glm::quat rot = transform.Rotation;
	rot = glm::rotate(rot, angle, glm::vec3(0.0f, 1.0f, 0.0f));
	transform.SetRotation(rot);

	RigidBody* rigidbody = GetComponent<RigidBody>();
	if (rigidbody)
		rigidbody->SetRotation(rot);
}
void Entity::Rotate(glm::vec3 dir, float Amount)
{
	Amount = glm::radians(Amount);
	glm::quat rot = transform.Rotation;
	rot = glm::rotate(rot, Amount, dir);
	transform.SetRotation(rot);

	RigidBody* rigidbody = GetComponent<RigidBody>();
	if (rigidbody)
		rigidbody->SetRotation(rot);
}
// --------------------------------------------------------------------------------------------
void Entity::OnDestory(bool isPlaying)
{
	for (size_t i = 0; i < m_scripts.size(); i++)
	{
		if (isPlaying && m_scripts[i]->hasDestroy)
			m_scripts[i]->CallMethod("OnDestroy");
		m_scripts[i]->GC = true;
	}

	for (size_t i = 0; i < m_components.size(); i++)
		m_components[i]->removed = true;

	RemoveAllChilds();
}
// --------------------------------------------------------------------------------------------
bool Entity::AddScript(ScriptInstance* scr)
{
	for (size_t i = 0; i < m_scripts.size(); i++)
	{
		if (m_scripts[i]->class_name == scr->class_name)
			return false;
	}
	m_scripts.push_back(scr);
	return true;
}
// --------------------------------------------------------------------------------------------
bool Entity::RemoveScript(std::string& scrName)
{
	for (size_t i = 0; i < m_scripts.size(); i++)
	{
		if (m_scripts[i]->class_name == scrName)
		{
			if (m_scripts[i]->hasDestroy) {
				m_scripts[i]->CallMethod("OnDestroy");
			}
			m_scripts[i]->GC = true;
			m_scripts.erase(m_scripts.begin() + i);
			return true;
		}
	}
	return false;
}
// --------------------------------------------------------------------------------------------
ScriptInstance* Entity::GetScript(std::string& scrName)
{
	for (size_t i = 0; i < m_scripts.size(); i++)
	{
		if (m_scripts[i]->class_name == scrName)
			return m_scripts[i];
	}
	return nullptr;
}
// --------------------------------------------------------------------------------------------
int Entity::GetScriptInstance(std::string & scrName)
{
	ScriptInstance* sc = GetScript(scrName);
	if (sc)
		return sc->ref_idx;
	return -1;
}
// --------------------------------------------------------------------------------------------
void Entity::StartScripts()
{
	for (size_t i = 0; i < m_scripts.size(); i++)
		if(m_scripts[i]->hasStart)
			m_scripts[i]->CallMethod("OnStart");
}
void Entity::DontDestroyOnLoad()
{
	if(parent != nullptr && parent->IsRoot())
		dontDestroyOnLoad = !dontDestroyOnLoad;
}
// --------------------------------------------------------------------------------------------
void Entity::InvokeScriptFunc(const char* funcName)
{
	for (size_t i = 0; i < m_scripts.size(); i++) 
		if (m_scripts[i]->HasFunc(funcName))
			m_scripts[i]->CallMethod(funcName);
}
// --------------------------------------------------------------------------------------------
void Entity::Unpack()
{
	is_prefab = false;
	path = "";
}
// --------------------------------------------------------------------------------------------
int Entity::GetObjIndex(EnttID obj_ID)
{
	for (size_t i = 0; i < ChildCount(); i++)
	{
		if (Children[i]->ID == obj_ID)
		{
			return i;
		}
	}
	return -1;
}
// --------------------------------------------------------------------------------------------
void Entity::DestroySelfIn(float tm)
{
	willDestroy = true;
	destroyIn = tm;
}
// --------------------------------------------------------------------------------------------
void Entity::DestroySelf()
{
	if (!root && parent != nullptr)
	{
		parent->RemoveChild(ID);
	}
}
// --------------------------------------------------------------------------------------------
void Entity::RemoveChild(EnttID o_id)
{
	transform.m_Dirty = true;

	for(size_t i = 0; i < Children.size();i++)
	{
		if(Children[i]->ID == o_id)
		{
			Children[i]->OnDestory();
			Entity* ent = Children[i];
			Children.erase(Children.begin() + GetObjIndex(o_id));
			delete ent;
			ent = nullptr;
			return;
		}
	}
}
// --------------------------------------------------------------------------------------------
void Entity::RemoveAllChilds(bool isPlaying)
{
	transform.m_Dirty = true;

	for (size_t i = 0; i < Children.size();i++)
	{
		if (/*isPlaying &&*/ Children[i]->dontDestroyOnLoad == true) continue;

		Children[i]->OnDestory(isPlaying);
		delete Children[i];
		Children[i] = nullptr;
	}
	Children.clear();
}
// --------------------------------------------------------------------------------------------
void Entity::RemoveChild_F(EnttID o_id)
{
	transform.m_Dirty = true;

	for (size_t i = 0; i < Children.size();i++)
	{
		if (Children[i]->ID == o_id)
		{
			Entity* ent = Children[i];
			Children.erase(Children.begin() + GetObjIndex(o_id));
			ent = nullptr;
			return;
		}
	}
}
// --------------------------------------------------------------------------------------------
void Entity::MoveChildPlace(EnttID entId, bool up)
{
	for (size_t i = 0; i < Children.size();i++)
	{
		if (Children[i]->ID == entId)
		{
			if (up)
			{
				if (i <= 0) return; // if child is on top, dont move it up
				Entity* trgt = Children[i];
				Children[i] = Children[i - 1];
				Children[i - 1] = trgt;
			}
			else
			{
				if (i >= (Children.size() - 1)) return; // if child is on back, dont move it down
				Entity* trgt = Children[i];
				Children[i] = Children[i + 1];
				Children[i + 1] = trgt;
			}
			return;
		}
	}
}
// --------------------------------------------------------------------------------------------
void Entity::MovePlace(bool up)
{
	if (parent != nullptr)
	{
		parent->MoveChildPlace(ID ,up);
	}
}
// --------------------------------------------------------------------------------------------
bool Entity::SetParent(Entity* target)
{
	if (target == nullptr)
		return false;

	target->AddChild(this);
	return true;
}
// --------------------------------------------------------------------------------------------
Entity* Entity::AddChild(Entity* mObj)
{
	if (mObj == nullptr) return nullptr;

	transform.m_Dirty = true;

	// Check if this child already has a parent. If so, first remove this entity from its current parent.
	if (mObj->parent)
	{
		mObj->parent->RemoveChild_F(mObj->ID);
	}
	mObj->parent = this;
	mObj->transform.parent = &transform;

	// reset position and scale, so the entity visual transform keep the same.
	mObj->transform.SetPosition(mObj->transform.Position - transform.GetWorldPosition());
	//mObj->transform.SetScale(mObj->transform.Scale / transform.Scale);

	Children.push_back(mObj);
	return Children[Children.size() - 1];
}
// --------------------------------------------------------------------------------------------
Entity* Entity::AddChild(std::string ent_name, EnttID ent_id)
{
	Entity* obj = new Entity();
	obj->name = ent_name;
	obj->ID = ent_id;
	obj->parent = this;
	obj->transform.parent = &transform;

	Children.push_back(obj);
	return obj;
}
// --------------------------------------------------------------------------------------------
Entity* Entity::Find_entity(EnttID obj_id, Entity* in_obj)
{
	// If we are looking for this entity, return it.
	if (in_obj->ID == obj_id)
		return in_obj;

	if (!in_obj->Children.empty())
	{
		for (size_t i = 0; i < in_obj->Children.size(); i++)
		{
			// if found then return it.
			Entity *obje = Find_entity(obj_id, in_obj->Children[i]);
			if (obje && obje->ID == obj_id)
				return obje;
		}
	}
	return nullptr;
} 
// --------------------------------------------------------------------------------------------
Entity* Entity::Find_entityN(std::string obj_name, Entity* in_obj)
{
	// If we are looking for this entity, return it.
	if (in_obj->name == obj_name)
		return in_obj;

	if (!in_obj->Children.empty())
	{
		for (size_t i = 0; i < in_obj->Children.size(); i++)
		{
			// if found then return it.
			Entity *obje = Find_entityN(obj_name, in_obj->Children[i]);
			if (obje != nullptr && obje->name == obj_name)
				return obje;
		}
	}
	return nullptr;
}

// --------------------------------------------------------------------------------------------
bool Entity::HasChild(EnttID child_id)
{
	for (size_t i = 0; i < ChildCount(); i++)
	{
		if (Children[i]->ID == child_id)
		{
			return true;
		}
	}
	return false;
}
// --------------------------------------------------------------------------------------------
bool Entity::SaveEntityFile(const char* pth, bool justCopy)
{
	if(!justCopy)
		is_prefab = false;

	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
	SaveBP(out, false);
	out << YAML::EndSeq;
	out << YAML::EndMap;

	RYAML::EndSave(out, pth);

	if (!justCopy) {
		is_prefab = true;
		path = pth;
	}
	return true;
}
// --------------------------------------------------------------------------------------------
Entity* Entity::GetChild(std::string obj_name)
{
	for (size_t i = 0; i < Children.size(); i++)
	{
		if (Children[i]->name == obj_name)
			return Children[i];
	}
	return nullptr;
}
// --------------------------------------------------------------------------------------------
Entity* Entity::GetChildWithTag(Entity* in_obj, std::string obj_tag)
{
	// If we are looking for this entity, return it.
	if (in_obj->tag == obj_tag)
		return in_obj;

	if (!in_obj->Children.empty())
	{
		for (size_t i = 0; i < in_obj->Children.size(); i++)
		{
			// if found then return it.
			Entity *obje = GetChildWithTag(in_obj->Children[i], obj_tag);
			if (obje && obje->tag == obj_tag)
				return obje;
		}
	}
	return nullptr;
}
// --------------------------------------------------------------------------------------------
Entity* Entity::GetParentWithTag(Entity* in_obj, std::string obj_tag)
{
	// If we are looking for this entity, return it.
	if (in_obj->tag == obj_tag)
		return in_obj;

	if (!in_obj->IsRoot())
	{
		return in_obj->GetParentWithTag(in_obj->parent, obj_tag);
	}
	return nullptr;
}
// --------------------------------------------------------------------------------------------
void Entity::BroadcastFunc(Entity * in_obj, std::string func_name)
{
	in_obj->InvokeScriptFunc(func_name.c_str());
	for (size_t i = 0; i < in_obj->ChildCount(); i++)
	{
		in_obj->Children[i]->BroadcastFunc(in_obj->Children[i], func_name);
	}
}
// --------------------------------------------------------------------------------------------
bool Entity::IsPrefab()
{
	return this->path != "";
}
// --------------------------------------------------------------------------------------------
// Data Serialization
// --------------------------------------------------------------------------------------------
void Entity::SaveBP(YAML::Emitter& out, bool is_root)
{
	out << YAML::BeginMap;
	out << YAML::Key << "Entity" << YAML::Value << ID;
	out << YAML::Key << "Root" << YAML::Value << is_root;
	if (!is_root && IsPrefab() && is_prefab)
	{
		//std::cout << "SAVING A PREFAB IN SCENE : " << path << std::endl;
		out << YAML::Key << "IsPrefab" << YAML::Value << true;
		out << YAML::Key << "name" << YAML::Value << name;
		out << YAML::Key << "Active" << YAML::Value << Active;
		out << YAML::Key << "Static" << YAML::Value << Static;
		out << YAML::Key << "m_CollMode" << YAML::Value << (int)m_CollMode;
		transform.SerializeSave(out);
		out << YAML::Key << "path" << YAML::Value << path;
		out << YAML::EndMap;
	}
	else
	{
		out << YAML::Key << "IsPrefab" << YAML::Value << false;
		out << YAML::Key << "name" << YAML::Value << name;
		out << YAML::Key << "tag" << YAML::Value << tag;
		out << YAML::Key << "Active" << YAML::Value << Active;
		out << YAML::Key << "Static" << YAML::Value << Static;
		out << YAML::Key << "m_CollMode" << YAML::Value << (int)m_CollMode;
		transform.SerializeSave(out);

		// serialize components
		// ------------------------------
		size_t comp_count = m_components.size();
		out << YAML::Key << "comp_count" << YAML::Value << m_components.size();

		out << YAML::Key << "Components" << YAML::Value << YAML::BeginSeq;
		for (size_t i = 0; i < comp_count; i++)
		{
			Component* comp = m_components[i];
			out << YAML::BeginMap;
			comp->OnSave(out);
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;

		out << YAML::Key << "script_count" << YAML::Value << m_scripts.size();
		out << YAML::Key << "Scripts" << YAML::Value << YAML::BeginSeq;
		for (size_t i = 0; i < m_scripts.size(); i++)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "class_name" << YAML::Value << m_scripts[i]->class_name;
			ScriptSerializer::SerializeScriptObject(out, m_scripts[i]);
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;

		// serialize Children entities
		// ------------------------------
		out << YAML::Key << "ChildCount" << YAML::Value << ChildCount();
		out << YAML::EndMap;

		if (!Children.empty())
		{
			for (size_t i = 0; i < Children.size(); i++)
			{
				Children[i]->SaveBP(out, false);
			}
		}
	}
	
}
// --------------------------------------------------------------------------------------------
