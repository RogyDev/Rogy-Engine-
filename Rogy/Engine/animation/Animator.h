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

	void Animator::UpdateAnimation(float dt, float scale = 1.0f)
	{
		m_DeltaTime = dt;
		if (m_CurrentAnimation)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
			float mintime = m_CurrentAnimation->GetDuration();
			if(mintime > m_CurrentAnimation2->GetDuration())mintime = m_CurrentAnimation2->GetDuration();
			m_CurrentTime = fmod(m_CurrentTime, mintime);
			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), &m_CurrentAnimation2->GetRootNode(), glm::scale(glm::mat4(1.0f), glm::vec3(scale)), scale);
		}
	}

	void Animator::PlayAnimation(Animation* pAnimation)
	{
		static bool first = true;
		m_CurrentAnimation2 = pAnimation;
		if (first) {
			first = false;
			m_CurrentAnimation = pAnimation;
		}
		

		m_CurrentTime = 0.0f;

		if (!loaded) {
			loaded = true;
			m_FinalBoneMatrices.reserve(100);

			for (int i = 0; i < 100; i++)
				m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
		}
	}

	glm::mat4 UpdateAnim(Bone* bone, float ctime)
	{
		glm::vec3 pos = bone->InterpolatePositionVec3(ctime);
		glm::quat rot = bone->InterpolateRotationQuat(ctime);
		glm::vec3 scl = bone->InterpolateScalingVec3(ctime);
		glm::mat4 mpos = glm::translate(glm::mat4(1.0f), pos);
		glm::mat4 mrot = glm::toMat4(rot);
		glm::mat4 mscl = glm::translate(glm::mat4(1.0f), scl);
		return (mpos * mrot * mscl);
	}

	glm::mat4 UpdateAnim(Bone* bone, Bone* bone2, float Blend, float ctime)
	{
		glm::vec3 pos = glm::mix(bone->InterpolatePositionVec3(ctime), bone2->InterpolatePositionVec3(ctime), Blend);
		glm::quat rot = glm::mix(bone->InterpolateRotationQuat(ctime), bone2->InterpolateRotationQuat(ctime), Blend);
		glm::vec3 scl = glm::mix(bone->InterpolateScalingVec3(ctime), bone2->InterpolateScalingVec3(ctime), Blend);
		glm::mat4 mpos = glm::translate(glm::mat4(1.0f), pos);
		glm::mat4 mrot = glm::toMat4(rot);
		glm::mat4 mscl = glm::translate(glm::mat4(1.0f), scl);
		return (mpos * mrot * mscl);
	}

	void Animator::CalculateBoneTransform(const AssimpNodeData* node, const AssimpNodeData* node2, glm::mat4 parentTransform, float scale)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* bone = m_CurrentAnimation->FindBone(nodeName);
		Bone* bone2 = m_CurrentAnimation2->FindBone(nodeName);

		if (bone && bone2)
		{
			nodeTransform = UpdateAnim(bone, bone2, 0.5f, m_CurrentTime);
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			m_FinalBoneMatrices[index] = globalTransformation * offset;

			if (nodeName == "wp") {
				wp = globalTransformation;
				glm::vec3 translation = glm::vec3(globalTransformation[3]);
				std::cout << "RightHand :" ;
				std::cout << " x :" << translation.x;
				std::cout << " y :" << translation.y;
				std::cout << " z :" << translation.z;
				std::cout << "\n";
			}
		}

		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], &node2->children[i], globalTransformation, scale);
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
	Animation* m_CurrentAnimation2;
	float m_CurrentTime;
	float m_DeltaTime;
	bool loaded = false;
};

#endif // ! SK_ANIMATOR_H