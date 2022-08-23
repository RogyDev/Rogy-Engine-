#ifndef R_COMPONENT_H
#define R_COMPONENT_H

#include <vector>
#include <iostream>
#include "../io/FSerializer.h"
//#include <yaml-cpp/yaml.h>
//#include <cereal/archives/binary.hpp>

template<typename T>
class ComponentArray 
{
public:
	ComponentArray()
	{

	}
	~ComponentArray()
	{
		for (size_t i = 0; i < components.size(); i++)
		{
			delete components[i];
		}
		components.clear();
	}

	void ClearRemovedComponents()
	{
		for (size_t i = 0; i < components.size(); i++)
		{
			if (components[i]->removed)
			{
				T* comp = components[i];
				components.erase(components.begin() + i);
				delete comp;
			}
		}
	}

	T* GetComponent(unsigned int entity_id)
	{
		for (size_t i = 0; i < components.size(); i++)
		{
			if (components[i]->entid == entity_id)
			{
				return components[i];
			}
		}
		return nullptr;
	}

	T* AddComponent(unsigned int entity_id)
	{
		for (size_t i = 0; i < components.size(); i++)
		{
			if (components[i]->entid == entity_id)
				return nullptr;
		}

		T* comp = new T();
		comp->entid = entity_id;
		components.push_back(comp);
		return comp;
	}

	bool RemoveComponent(unsigned int entity_id)
	{
		for (size_t i = 0; i < components.size(); i++)
		{
			if (components[i]->entid == entity_id)
			{
				T* comp = components[i];
				components.erase(components.begin() + i);
				delete comp;
				return true;
			}
		}
		return false;
	}

	size_t Size()
	{
		return components.size();
	}

	bool Empty()
	{
		return components.empty();
	}

	std::vector<T*> GetComponents()
	{
		return components;
	}
	
	std::vector<T*> components;
};


/* Must be added inside class header for the static reflection system to work properly. */
#define BASE_COMPONENT()\
public: \
	static int TYPE_ID;\

/* Must be added inside class source implmentation file for the component. */
#define IMPL_COMPONENT(Tn)\
	int Tn::TYPE_ID = 0;\

class Component
{
public:
	Component();
	~Component();

	virtual void OnSave(YAML::Emitter& out) {}
	virtual void OnLoad(YAML::Node& data) {}

	static int IndxID;

	template<typename T>
	static void RegisterComponent()
	{
		T::TYPE_ID = IndxID;
		IndxID = IndxID + 1;
		//std::cout << "RegisterComponent -> " << T::TYPE_ID << std::endl;
	};

	/* Cast component to a derived type and check for nullptr or type*/
	template<typename T>
	static T* ComponentCast(Component* comp)
	{
		if (comp && T::TYPE_ID == comp->type_id)
			return static_cast<T*>(comp);
		return nullptr;
	};

	/* Cast component to a derived type without nullptr or type checking*/
	template<typename T>
	static T* QuickCast(Component* comp)
	{
		return static_cast<T*>(comp);
	};

	/* Check or type*/
	template<typename T>
	static bool IsComponentType(Component* comp)
	{
		if (T::TYPE_ID == comp->type_id)
			return true;
		return false;
	};

	int type_id;
	unsigned int entid;
	bool removed = false;
	bool enabled = true;
	void* entity;
private:

};

// Register a new componet type to the component-based system
#define REGISTER_COMPONENT(T)\
	Component::RegisterComponent<T>()\

#endif // R_COMPONENT_H