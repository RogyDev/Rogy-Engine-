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

Texture* ResourcesManager::CreateTexture(std::string tex_name, const char* tex_path, bool flip, bool keepdata)
{
	Texture* tex = GetTexture(tex_path);
	if (tex != nullptr)
		return tex;

	Texture* texp = new Texture();
	if (!texp->setTexture(tex_path, tex_name, flip, keepdata))
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

Animation* ResourcesManager::GetAnimation(std::string animName, std::string animPath, SK_Model* mesh, unsigned int at_index)
{
	for (size_t i = 0; i < mAnimations.size(); i++)
	{
		if (mAnimations[i]->anim_name == animName)
			return mAnimations[i];
	}

	Animation* anim = new Animation;
	anim->anim_name = animName;
	if (!anim->LoadAnimation(animPath, mesh, at_index))
	{
		delete anim;
		return nullptr;
	}

	mAnimations.push_back(anim);
	return anim;
	
}
