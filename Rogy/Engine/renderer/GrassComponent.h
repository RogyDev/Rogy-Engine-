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


	// Serialization
	virtual void OnSave(YAML::Emitter& out) override
	{
		out << YAML::Key << "GrassComponent" << YAML::BeginMap;

		out << YAML::Key << "enabled" << YAML::Value << enabled;
		out << YAML::Key << "alpha" << YAML::Value << alpha;
		out << YAML::Key << "sizex" << YAML::Value << size.x;
		out << YAML::Key << "sizey" << YAML::Value << size.y;
		out << YAML::Key << "Distance" << YAML::Value << Distance;
		if (mTexture != nullptr)	texPath = mTexture->getTexName();
		out << YAML::Key << "texPath" << YAML::Value << texPath;
		out << YAML::Key << "mGrass_size" << YAML::Value << mGrass.size();

		out << YAML::Key << "mGrass" << YAML::Value << YAML::BeginSeq;
		out << YAML::Flow;
		for (size_t i = 0; i < mGrass.size(); i++)
		{
			out << mGrass[i].x;
			out << mGrass[i].y;
			out << mGrass[i].z;
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;
	}

	virtual void OnLoad(YAML::Node& data) override
	{
		enabled = data["enabled"].as<bool>();
		alpha = data["alpha"].as<float>();
		size.x = data["sizex"].as<float>();
		size.y = data["sizey"].as<float>();
		Distance = data["Distance"].as<float>();
		texPath = data["texPath"].as<std::string>();
		size_t length = data["mGrass_size"].as<unsigned int>();
		
		auto mgrass = data["mGrass"];
		glm::vec3 poss;
		for (size_t i = 0; i < length; i+=3)
		{
			poss.x = mgrass[i].as<float>();
			poss.y = mgrass[i+1].as<float>();
			poss.z = mgrass[i+2].as<float>();
			mGrass.push_back(glm::vec3(poss.x, poss.y, poss.z));
		}
	}

private:
};

#endif // RGRASS_COMPONENT_H