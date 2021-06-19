#ifndef SK_ANIMATOR_H
#define SK_ANIMATOR_H

#include "Animation.h"

class Animator
{
public:

	Animator::Animator()
	{
	}

	Animator::Animator(Animation* currentAnimation)
	{
		LoadAnimation(currentAnimation);
	}

	bool HasAnimation()
	{
		return (m_CurrentAnimation != nullptr);
	}

	void LoadAnimation(Animation* currentAnimation)
	{
		m_CurrentTime = 0.0;
		m_CurrentAnimation = currentAnimation;

		if (!loaded) {
			loaded = true;
			m_FinalBoneMatrices.reserve(100);

			for (int i = 0; i < 100; i++)
				m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
		}
	}

	glm::mat4 GetNodeTransform(const AssimpNodeData* node, std::string& node_name, bool& is_found)
	{
		if (node->name == node_name) {
			is_found = true;
			return node->transformation;
		}

		for (size_t i = 0; i < node->children.size(); i++)
		{
			bool found = false;
			glm::mat4 mt = GetNodeTransform(&node->children[i], node_name, found);
			if (found) { is_found = true; return mt; }
		}
		return glm::mat4(0.0f);
	}

	void Animator::UpdateAnimation(float dt)
	{
		m_DeltaTime = dt;
		if (m_CurrentAnimation)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}

	void Animator::PlayAnimation(Animation* pAnimation)
	{
		m_CurrentAnimation = pAnimation;
		m_CurrentTime = 0.0f;

		if (!loaded) {
			loaded = true;
			m_FinalBoneMatrices.reserve(100);

			for (int i = 0; i < 100; i++)
				m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
		}
	}

	void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

		if (Bone)
		{
			Bone->Update(m_CurrentTime);
			nodeTransform = Bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		if (nodeName == "wp")
			wp = nodeTransform;

		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			m_FinalBoneMatrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}

	std::vector<glm::mat4> GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}

	const AssimpNodeData* GetAnimationRootNode()
	{
		return &m_CurrentAnimation->GetRootNode();
	}
	glm::mat4 wp;

private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;
	bool loaded = false;
};

#endif // ! SK_ANIMATOR_H