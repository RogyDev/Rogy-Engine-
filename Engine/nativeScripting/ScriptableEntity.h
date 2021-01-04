#ifndef SCRIPTABLE_ENTITY_H
#define SCRIPTABLE_ENTITY_H

#include "../scene/Entity.h"

class ScriptableEntity
{
public:
	virtual ~ScriptableEntity() {}

	template<typename T>
	T& GetComponent()
	{
		return m_entity->GetComponent<T>();
	}

	virtual void OnCreate() {}
	virtual void OnDestroy() {}
	virtual void OnUpdate(float ts) {}

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

#endif // SCRIPTABLE_ENTITY_H