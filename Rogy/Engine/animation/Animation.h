#ifndef SK_ANIMATION_H
#define SK_ANIMATION_H

#include <glm/glm.hpp>
#include <vector>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assert.h>
#include <map>
#include "SkeletalModel.h"

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation
{
public:
	std::string anim_name;
	std::string anim_path;
	unsigned int anim_index;

	Animation() = default;

	Animation(const std::string& animationPath, SK_Model* model)
	{
		LoadAnimation(animationPath, model);
	}

	~Animation()
	{
		//std::cout << "Animation Deleted " << anim_name << std::endl;
	}

	bool LoadAnimation(const std::string& animationPath, SK_Model* model, int at_index = 0)
	{
		anim_path = animationPath;
		//std::cout << "Animation loading " << anim_name << " |in| " << animationPath << std::endl;
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
		if (!scene || !scene->mRootNode)
			return false;
		auto animation = scene->mAnimations[at_index];
		m_Duration = (float)animation->mDuration;
		m_TicksPerSecond = (int)animation->mTicksPerSecond;
		ReadHeirarchyData(m_RootNode, scene->mRootNode);
		ReadMissingBones(animation, *model);
		anim_index = at_index;
		return true;
	}

	/*Bone* FindBone(const std::string& name)
	{
		for (size_t i = 0; i < m_Bones.size(); i++)
		{
			if (m_Bones[i].GetBoneName() == name)
				return &m_Bones[i];
		}
		return nullptr;
	}*/

	Bone* FindBone(const std::string& name)
	{
		auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
			[&](const Bone& Bone)
		{
			return Bone.GetBoneName() == name;
		}
		);
		if (iter == m_Bones.end()) return nullptr;
		else return &(*iter);
	}


	inline float GetTicksPerSecond() { return (float)m_TicksPerSecond; }

	inline float GetDuration() { return m_Duration; }

	inline const AssimpNodeData& GetRootNode() { return m_RootNode; }

	inline const std::map<std::string, BoneInfo>& GetBoneIDMap()
	{
		return m_BoneInfoMap;
	}

private:
	void ReadMissingBones(const aiAnimation* animation, SK_Model& model)
	{
		int size = animation->mNumChannels;

		auto& boneInfoMap = model.m_BoneInfoMap;//getting m_BoneInfoMap from Model class
		int& boneCount = model.m_BoneCounter; //getting the m_BoneCounter from Model class

		//reading channels(bones engaged in an animation and their keyframes)
		for (int i = 0; i < size; i++)
		{
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;

			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				boneInfoMap[boneName].id = boneCount;
				boneCount++;
			}
			m_Bones.push_back(Bone(channel->mNodeName.data,
				boneInfoMap[channel->mNodeName.data].id, channel));
		}

		m_BoneInfoMap = boneInfoMap;
	}

	void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src)
	{
		assert(src);

		dest.name = src->mName.data;
		aiMatrix4x4 mt = src->mTransformation;
		dest.transformation = SK_Model::AiToGLMMat4(mt);
		dest.childrenCount = src->mNumChildren;

		for (size_t i = 0; i < src->mNumChildren; i++)
		{
			AssimpNodeData newData;
			ReadHeirarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}
	float m_Duration;
	int m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
};

#endif // ! SK_SKELETAL_MODEL_H