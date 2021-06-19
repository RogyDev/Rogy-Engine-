#include "resources_manger.h"

ResourcesManager::ResourcesManager()
{	
}

ResourcesManager::~ResourcesManager()
{
}

void ResourcesManager::Init()
{
}

void ResourcesManager::Clear()
{
	mMeshs.Clear();
}

Texture* ResourcesManager::CreateTexture(std::string tex_name, const char* tex_path, bool flip)
{
	Texture* tex = GetTexture(tex_path);
	if (tex != nullptr)
		return tex;

	Texture* texp = new Texture();
	if (!texp->setTexture(tex_path, tex_name, flip)) 
	{
		delete texp;
		return nullptr;
	}
	mTextures.push_back(texp);
	return texp;
}
bool ResourcesManager::RemoveTexture(std::string tex_name)
{
	for (size_t i = 0; i < mTextures.size(); i++)
	{
		if (mTextures[i]->getTexPath() == tex_name)
		{
			mTextures.erase(mTextures.begin() + i);
			return true;
		}
	}
	return false;
}
Texture* ResourcesManager::GetTexture(std::string tex_name)
{
	for (size_t i = 0; i < mTextures.size(); i++)
	{
		if (mTextures[i]->getTexPath() == tex_name)
		{
			return mTextures[i];
		}
	}
	return nullptr;
}

Texture* ResourcesManager::GetTextureName(std::string tex_name)
{
	for (size_t i = 0; i < mTextures.size(); i++)
	{
		if (mTextures[i]->getTexName() == tex_name)
		{
			return mTextures[i];
		}
	}
	return nullptr;
}
