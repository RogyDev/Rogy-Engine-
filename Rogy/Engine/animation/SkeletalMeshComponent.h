#ifndef SKELETAL_MESH_COMPONENT_H
#define SKELETAL_MESH_COMPONENT_H

#include "../animation/Animator.h"
#include "../scene/RComponent.h"
#include <glm/glm.hpp>
#include <string>
#include "../mesh/model.h"
#include "../shading/material.h"

class SkeletalMeshComponent : public Component
{
	BASE_COMPONENT()

	SkeletalMeshComponent();
	~SkeletalMeshComponent();

	float scale = 1.0f;

	glm::mat4 transform;
	glm::vec3 position;

	SK_Model*      mesh     = nullptr;
	Material*  material = nullptr;
	std::vector<Material*> materials;
	
	bool CastShadows = true;

	Animator animator;
	std::vector<Animation*> animations;

	std::vector<unsigned int> skip_meshs;
	void SkipMesh(unsigned int at_index);
	void UnSkipMesh(unsigned int at_index);
	bool isMeshSkiped(unsigned int at_index);

	void LoadAnimation(std::string anim_name, std::string anim_path, unsigned int at_index = 0);
	void RemoveAnimation(std::string anim_name);
	void PlayAnimation(std::string anim_name);
	void Update(float dt);
	void Clear();
	void Draw();

	// Serialization
	virtual void OnSave(YAML::Emitter& out) override
	{
		out << YAML::Key << "SkeletalMeshComponent" << YAML::BeginMap;

		out << YAML::Key << "enabled" << YAML::Value << enabled;
		out << YAML::Key << "CastShadows" << YAML::Value << CastShadows;
		out << YAML::Key << "scale" << YAML::Value << scale;

		out << YAML::Key << "mat_name" << YAML::Value << material->getMatName();
		out << YAML::Key << "mat_source" << YAML::Value << material->getMatPath();
		out << YAML::Key << "HasMesh" << YAML::Value << (mesh != nullptr);
		if (mesh != nullptr) {
			out << YAML::Key << "mesh_source" << YAML::Value << mesh->mesh_dir;
		}

		out << YAML::Key << "anim_size" << YAML::Value << animations.size();
		out << YAML::Key << "mAnims" << YAML::Value << YAML::BeginSeq;
		out << YAML::Flow;
		for (size_t i = 0; i < animations.size(); i++)
		{
			out << animations[i]->anim_name;
			out << animations[i]->anim_path;
			out << animations[i]->anim_index;
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;
	}

	virtual void OnLoad(YAML::Node& data) override
	{
		enabled = data["enabled"].as<bool>();
		CastShadows = data["CastShadows"].as<bool>();
		scale = data["scale"].as<float>();

		size_t length = data["anim_size"].as<unsigned int>();

		auto mAnims = data["mAnims"];
		std::string aname, asrc;
		size_t aindx;
		for (size_t i = 0; i < length; i += 3)
		{
			aname = mAnims[i].as<std::string>();
			asrc = mAnims[i + 1].as<std::string>();
			aindx = mAnims[i + 2].as<int>();
			LoadAnimation(aname, asrc);
		}
	}
};


#endif // SKELETAL_MESH_COMPONENT_H