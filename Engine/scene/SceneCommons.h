#ifndef SCENE_COMMMONS_H
#define SCENE_COMMMONS_H

#include <vector>

// A way to tell the systems that an entity has been destroyed or has removed its component

class SceneCommons
{
public:
	struct RemovedComponent
	{
		unsigned int type_id;
		std::vector<unsigned int> entities;

		RemovedComponent(int typeID)
		{
			type_id = typeID;
		}
	};

	static std::vector<RemovedComponent> DestroyedEntities;
	static void OnEntityRemovedComponent(unsigned int ent_id, int componet_type_id);
	static std::vector<unsigned int> GetRemovedEntitiesFromType(int componet_type_id);
	static void Clear();
};

#endif //SCENE_COMMMONS_H