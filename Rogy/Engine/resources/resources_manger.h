#ifndef RES_MANAGER_H
#define RES_MANAGER_H

#include <string>
#include <iostream>

#include "../animation/Animation.h"
#include "../shading/texture.h"
#include "modelmanger.h"
#include "../scene/Scene.h"

#include "asset_path.h"

class ResourcesManager
{
public:
	ResourcesManager();
	~ResourcesManager();

	SceneManager* scene;

	ModelManager mMeshs;

	std::vector<Animation*> mAnimations;

	std::vector<Texture*> mTextures;

	void Init();
	void Clear();

	Texture* CreateTexture(std::string tex_name, const char* tex_path, bool flip = false, bool keepdata = false);
	bool	 RemoveTexture(std::string tex_name);
	Texture* GetTexture(std::string tex_name);
	Texture* GetTextureName(std::string tex_name);

	Animation* GetAnimation(std::string animName, std::string animPath, SK_Model* mesh, unsigned int at_index = 0);
	
private:

};

#endif