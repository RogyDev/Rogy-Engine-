#include "modelmanger.h"

// --------------------------------------------------------------
ModelManager::ModelManager()
{

}
// --------------------------------------------------------------
ModelManager::~ModelManager()
{

}
// --------------------------------------------------------------
Model* ModelManager::CreateModel(std::string& path)
{
	for (size_t i = 0; i < items.size(); i++)
	{
		if (items[i]->mesh_dir == path)
		{
			return items[i];
		}
	}
	Model* mdl = new Model(path, counterID);

	items.push_back(mdl);

	counterID++;

	return mdl;
}
// --------------------------------------------------------------
int ModelManager::loadModel(Model* m_id, string path)
{
	Model* already_m = nullptr;

	bool model_found = false;
	for (size_t i = 0; i < items.size(); i++)
	{
		if (items[i]->id != m_id->id && items[i]->mesh_dir == path)
		{
			already_m = items[i];
			model_found = true;
		}
	}

	if (model_found)
	{
		if (already_m->id != m_id->id)
		{
			//std::cout << "and not exist " << m_id->id << endl;
			RemoveModel(m_id->id);
			//m_id = already_m;

			return already_m->id;
		}
	}
	else
	{
		m_id->loadModel(path);
	}
	return -1;
}
// --------------------------------------------------------------
Model* ModelManager::GetModel(int item_id)
{
	for (size_t i = 0; i < items.size(); i++)
	{
		if (items[i]->id == item_id)
		{
			return items[i];
		}
	}
	return nullptr;
}
// --------------------------------------------------------------

bool ModelManager::RemoveModel(int item_id)
{
	for (size_t i = 0; i < items.size(); i++)
	{
		if (items[i]->id == item_id)
		{
			items.erase(items.begin() + i);
			items.shrink_to_fit();

			return true;
		}
	}
	return false;
}
// --------------------------------------------------------------
void ModelManager::Clear()
{
	for (size_t i = 0; i < items.size(); i++)
	{
		items[i]->clear();
	}

	items.clear();
	items.shrink_to_fit();
}
// --------------------------------------------------------------
SK_Model* ModelManager::LoadSkeletalModel(std::string path)
{
	for (size_t i = 0; i < skeletalModels.size(); i++)
	{
		if (skeletalModels[i]->mesh_dir == path)
			return skeletalModels[i];
	}

	SK_Model* m = new SK_Model;
	if (!m->loadModel(path))
	{
		std::cout << "skeletal mesh loading failed!\n";
		delete m;
		return nullptr;
	}
	skeletalModels.push_back(m);
	return m;
}
// --------------------------------------------------------------