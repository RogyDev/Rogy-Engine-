#include "SceneCommons.h"

std::vector<SceneCommons::RemovedComponent> SceneCommons::DestroyedEntities;

void SceneCommons::OnEntityRemovedComponent(unsigned int ent_id, int componet_type_id)
{
	for (size_t i = 0; i < DestroyedEntities.size(); i++)
	{
		if (DestroyedEntities[i].type_id == componet_type_id)
		{
			DestroyedEntities[i].entities.push_back(ent_id);
			return;
		}
	}
	//DestroyedEntities.push_back(RemovedComponent(componet_type_id));
	DestroyedEntities.emplace_back(componet_type_id);
	OnEntityRemovedComponent(ent_id, componet_type_id);
}

std::vector<unsigned int> SceneCommons::GetRemovedEntitiesFromType(int componet_type_id)
{
	for (size_t i = 0; i < DestroyedEntities.size(); i++)
	{
		if (DestroyedEntities[i].type_id == componet_type_id)
		{
			return DestroyedEntities[i].entities;
		}
	}
	return std::vector<unsigned int>();
}

void SceneCommons::Clear()
{
	DestroyedEntities.clear();
}