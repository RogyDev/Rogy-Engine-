#ifndef SK_ANIMATOR_H
#define SK_ANIMATOR_H

#include "Animation.h"

class Animator
{
public:

	Animator();
	Animator(Animation* currentAnimation);

	bool HasAnimation();
	void LoadAnimation(Animation* currentAnimation);
	void UpdateAnimation(float dt, float scale = 1.0f);
	void PlayAnimation(Animation* pAnimation);
	//void CalculateBoneTransform(std::vector<const AssimpNodeData*> node/*const AssimpNodeData* node, const AssimpNodeData* node2*/, glm::mat4 parentTransform, float scale);
	void  CalculateBoneTransform(const AssimpNodeData* node, const AssimpNodeData* node2, glm::mat4 parentTransform, float scale);

	glm::mat4 GetNodeTransform(const AssimpNodeData* node, std::string& node_name, bool& is_found);
	glm::mat4 UpdateAnim(Bone* bone, float ctime);
	glm::mat4 UpdateAnim(Bone* bone, Bone* bone2, float Blend, float ctime);
	std::vector<glm::mat4> GetFinalBoneMatrices();

	/*const AssimpNodeData* GetAnimationRootNode()
	{
		return &m_CurrentAnimation->GetRootNode();
	}*/

	glm::mat4 wp;

private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation = nullptr;
	Animation* m_CurrentAnimation2 = nullptr;

	//std::vector<Animation*> animations;
	//std::vector<float> animations_blend;

	float m_CurrentTime;
	float m_DeltaTime;
	bool loaded = false;
};

#endif // ! SK_ANIMATOR_H