#ifndef RGRASS_COMPONENT_H
#define RGRASS_COMPONENT_H

#include "../scene/RComponent.h"
#include <glm/glm.hpp>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <gl\glew.h>
#include "../core/Random.h"
#include "../shading/texture.h"

class GrassComponent : public Component
{
	BASE_COMPONENT()
public:
	GrassComponent() {}
	~GrassComponent() {}

	Texture* mTexture = nullptr;
	float alpha = 0.1f;
	glm::vec2 size = glm::vec2(1.0f);
	float Distance = 200.0f;

	std::string texPath;

	bool edit = false;
	unsigned int Edit_Raduis = 80;
	unsigned int Edit_amount = 20;
	
	void AddGrassBlade(glm::vec3& pos)
	{
		mGrass.emplace_back(pos);
	}
	void RemoveGrassBlade(size_t index)
	{
		mGrass.erase(mGrass.begin() + index);
	}

	void RemoveBladesInRange(glm::vec3 pos)
	{
		for (size_t i = 0; i < mGrass.size(); i++)
		{
			if (glm::distance(mGrass[i], pos) < Edit_Raduis)
				RemoveGrassBlade(i);
		}
	}

	size_t Size()
	{
		return mGrass.size();
	}

	std::vector<glm::vec3> mGrass;

	template <class Archive>
	void SerializeSave(Archive & ar)
	{
		ar(enabled);
		ar(alpha);
		ar(size.x, size.y);
		ar(Distance);
		if (mTexture != nullptr)
			texPath = mTexture->getTexName();
		ar(texPath);
		ar(mGrass.size());
		for (size_t i = 0; i < mGrass.size(); i++)
		{
			ar(mGrass[i].x);
			ar(mGrass[i].y);
			ar(mGrass[i].z);
		}
	}
	template <class Archive>
	void SerializeLoad(Archive & ar)
	{
		ar(enabled);
		ar(alpha);
		ar(size.x, size.y);
		ar(Distance);
		ar(texPath);
		size_t sizee = 0;
		ar(sizee);
		glm::vec3 poss;
		for (size_t i = 0; i < sizee; i++)
		{
			ar(poss.x);
			ar(poss.y);
			ar(poss.z);
			mGrass.push_back(glm::vec3(poss.x, poss.y, poss.z));
		}
	}
private:
};

#endif // RGRASS_COMPONENT_H