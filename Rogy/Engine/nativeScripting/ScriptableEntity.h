#ifndef SCRIPTABLE_ENTITY_H
#define SCRIPTABLE_ENTITY_H

#include "../scene/Entity.h"

class ScriptableEntity
{
public:
	enum NScriptVarType
	{
		NSVT_NULL = 0,
		NSVT_FLOAT = 1,
		NSVT_INT = 2,
		NSVT_STR = 3,
		NSVT_BOOL = 4
	};

	struct NScriptVar
	{
		NScriptVarType type = NScriptVarType::NSVT_NULL;
		std::string name;
		void* var = nullptr;

		NScriptVar()
		{
			type = NScriptVarType::NSVT_NULL;
		}

		NScriptVar(NScriptVarType t, const char* n, void* v)
		{
			var = v;
			name = n;
			type = t;
		}
	};

	virtual ~ScriptableEntity() {}

	template<typename T>
	T& GetComponent()
	{
		return m_entity->GetComponent<T>();
	}

	virtual void GetVar(const char* varName, NScriptVar& vvar) { }
	virtual void Init() {}
	virtual void Internal_Reflect() {}
	virtual void OnCreate() {}
	virtual void OnDestroy() {}
	virtual void OnUpdate(float ts) {}

	bool hasVar(const char* vn)
	{
		for (size_t i = 0; i < varsNames.size(); i++)
			if (varsNames[i] == vn)
				return true;
		return false;
	}
	std::vector<std::string> varsNames;
	std::vector<int> varsType;
	std::vector<void*> vars;
	Entity* m_entity;
private:
};

class NativeScriptComponent : public Component
{
	BASE_COMPONENT()
public:

	ScriptableEntity* Instance = nullptr;

	ScriptableEntity*(*InstantiateScript)();
	void(*DestroyScript)(NativeScriptComponent*);

	template<typename T>
	void Bind()
	{
		InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
		DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
	}
private:

};

#define RCLASS(vname)\
vname() { Internal_Reflect(); } \
virtual void Internal_Reflect() override\


#define RVARIABLE(vtype, vname, vval)\
varsNames.push_back(vname);\
vars.push_back(&vval);\
varsType.push_back((int)vtype);\

class CppTest : public ScriptableEntity
{
public:

	float speedd = 0.3f;
	int health = 100;
	std::string playerName = "susy";
	bool moreHealth = false;
	bool Die = false;

	RCLASS(CppTest)
	{
		RVARIABLE(NSVT_FLOAT, "speedd", speedd)
		RVARIABLE(NSVT_INT, "Health", health)
		RVARIABLE(NSVT_STR, "Player Name", playerName)
		RVARIABLE(NSVT_BOOL, "Add Health", moreHealth)
		RVARIABLE(NSVT_BOOL, "Die", Die)
	}

	virtual void OnCreate() override
	{
		std::cout << "OnCreate()\n";
	}

	virtual void OnDestroy() override
	{
	}

	virtual void OnUpdate(float dt) override
	{
		if (Die)
		{
			DestroyCube();
			return;
		}

		glm::vec3 pos = m_entity->transform.GetLocalPosition();
		pos.z += speedd;
		m_entity->transform.SetPosition(pos);

		m_entity->name = playerName;

		if (moreHealth)
		{
			health++;
			health = glm::clamp(health, 0, 100);
		}
	}

	void DestroyCube()
	{
		m_entity->DestroySelf();
	}
};

#endif // SCRIPTABLE_ENTITY_H