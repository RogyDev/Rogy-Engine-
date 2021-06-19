#ifndef  MODEL_MANAGER_H
#define  MODEL_MANAGER_H

#include <vector>
#include "../mesh/model.h"
#include "../animation/SkeletalModel.h"

class ModelManager 
{
public:
	ModelManager();
	~ModelManager();

	std::vector<Model*> items;
	std::vector<SK_Model*> skeletalModels;

	int counterID = 0;
	Model* CreateModel(std::string& path);
	int loadModel(Model* m_id, string path);
	bool RemoveModel(int item_id);
	Model* GetModel(int item_id);

	SK_Model* LoadSkeletalModel(std::string path);

	void Clear();
};

#endif // ! MODEL_MANAGER_H