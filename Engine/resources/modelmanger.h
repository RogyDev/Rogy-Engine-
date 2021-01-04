#ifndef  MODEL_MANAGER_H
#define  MODEL_MANAGER_H

#include <vector>
#include "../mesh/model.h"

class ModelManager 
{
public:
	ModelManager();
	~ModelManager();

	std::vector<Model*> items;

	int counterID = 0;

	Model* CreateModel(string path);

	int loadModel(Model* m_id, string path);

	bool RemoveModel(int item_id);

	Model* GetModel(int item_id);

	void Clear();
};

#endif // ! MODEL_MANAGER_H