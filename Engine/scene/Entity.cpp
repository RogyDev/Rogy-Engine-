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
void Entity::OnCollision(bool enter, Entity* trgt)
{
	for (size_t i = 0; i < m_scripts.size(); i++)
	{
		if (!m_scripts[i]->hasOnCollision) continue;
		m_scripts[i]->PrepareMethod("OnCollision");
		lua_pushboolean(m_scripts[i]->L, enter);
		lua_pushinteger(m_scripts[i]->L, trgt->ID);
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
	else
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
// --------------------------------------------------------------------------------------------
void Entity::OnDestory()
{
	for (size_t i = 0; i < m_scripts.size(); i++)
	{
		if (!m_scripts[i]->hasDestroy)
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
// --------------------------------------------------------------------------------------------
void Entity::InvokeScriptFunc(const char* funcName)
{
	for (size_t i = 0; i < m_scripts.size(); i++)
		m_scripts[i]->CallMethod(funcName);
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
void Entity::RemoveAllChilds()
{
	transform.m_Dirty = true;

	for (size_t i = 0; i < Children.size();i++)
	{
		Children[i]->OnDestory();
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
	mObj->transform.SetPosition(mObj->transform.Position - transform.Position);
	mObj->transform.SetScale(mObj->transform.Scale / transform.Scale);

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
	{
		return in_obj;
	}

	if (!in_obj->Children.empty())
	{
		for (size_t i = 0; i < in_obj->Children.size(); i++)
		{
			// if found then return it.
			Entity *obje = Find_entity(obj_id, in_obj->Children[i]);
			if (obje && obje->ID == obj_id)
			{
				return obje;
			}
		}
	}
	return nullptr;
} 
// --------------------------------------------------------------------------------------------
Entity* Entity::Find_entityN(std::string obj_name, Entity* in_obj)
{
	// If we are looking for this entity, return it.
	if (in_obj->name == obj_name)
	{
		return in_obj;
	}

	if (!in_obj->Children.empty())
	{
		for (size_t i = 0; i < in_obj->Children.size(); i++)
		{
			// if found then return it.
			Entity *obje = Find_entityN(obj_name, in_obj->Children[i]);
			if (obje != nullptr && obje->name == obj_name)
			{
				return obje;
			}
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
bool Entity::SaveEntityFile(const char* pth)
{
	std::ofstream os(pth, std::ios::binary);
	cereal::BinaryOutputArchive archive(os);
	SaveBP(archive, true);
	os.close();

	is_prefab = true;
	path = pth;

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
bool Entity::IsPrefab()
{
	return this->path != "";
}
// --------------------------------------------------------------------------------------------
// Data Serialization
// --------------------------------------------------------------------------------------------
void Entity::SaveBP(cereal::BinaryOutputArchive& ar, bool is_root)
{
	if (!is_root && IsPrefab())
	{
		std::cout << "SAVING A PREFAB IN SCENE : " << path << std::endl;
		ar(std::string(SER_VERSION));
		ar(true);
		ar(transform);
		ar(path);
	}
	else
	{
		ar(std::string(SER_VERSION));
		ar(false);
		ar(name);
		ar(tag);
		ar(Active);
		ar(Static);
		ar(transform);

		// serialize components
		// ------------------------------
		int num_comp = m_components.size();
		ar(num_comp);

		for (int i = 0; i < num_comp; i++)
		{
			Component* comp = m_components[i];
			ar(comp->type_id);

			if (Component::IsComponentType<RendererComponent>(comp))
				Component::QuickCast<RendererComponent>(comp)->SerializeSave<cereal::BinaryOutputArchive>(ar);

			else if (Component::IsComponentType<PointLight>(comp))
				Component::QuickCast<PointLight>(comp)->SerializeSave<cereal::BinaryOutputArchive>(ar);

			else if (Component::IsComponentType<ReflectionProbe>(comp))
				Component::QuickCast<ReflectionProbe>(comp)->SerializeSave<cereal::BinaryOutputArchive>(ar);

			else if (Component::IsComponentType<SpotLight>(comp))
				Component::QuickCast<SpotLight>(comp)->SerializeSave<cereal::BinaryOutputArchive>(ar);

			else if (Component::IsComponentType<DirectionalLight>(comp))
				Component::QuickCast<DirectionalLight>(comp)->SerializeSave<cereal::BinaryOutputArchive>(ar);

			else if (Component::IsComponentType<BillboardComponent>(comp))
				Component::QuickCast<BillboardComponent>(comp)->SerializeSave<cereal::BinaryOutputArchive>(ar);

			else if (Component::IsComponentType<CameraComponent>(comp))
				Component::QuickCast<CameraComponent>(comp)->SerializeSave<cereal::BinaryOutputArchive>(ar);

			else if (Component::IsComponentType<RigidBody>(comp))
				Component::QuickCast<RigidBody>(comp)->SerializeSave<cereal::BinaryOutputArchive>(ar);

			else if (Component::IsComponentType<ParticleSystem>(comp))
				Component::QuickCast<ParticleSystem>(comp)->SerializeSave<cereal::BinaryOutputArchive>(ar);

			else if (Component::IsComponentType<RAudioSource>(comp))
				Component::QuickCast<RAudioSource>(comp)->SerializeSave<cereal::BinaryOutputArchive>(ar);

			else if (Component::IsComponentType<GrassComponent>(comp))
				Component::QuickCast<GrassComponent>(comp)->SerializeSave<cereal::BinaryOutputArchive>(ar);
		}

		ar(m_scripts.size());
		for (size_t i = 0; i < m_scripts.size(); i++)
		{
			ar(m_scripts[i]->class_name);
			ScriptSerializer::SerializeScriptObject(ar, m_scripts[i]);
		}

		// serialize Children entities
		// ------------------------------
		ar(ChildCount());
		if (!Children.empty())
		{
			for (size_t i = 0; i < Children.size(); i++)
			{
				Children[i]->SaveBP(ar, false);
			}
		}
	}
}
// --------------------------------------------------------------------------------------------
