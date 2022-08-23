#include "SkeletalMeshComponent.h"
IMPL_COMPONENT(SkeletalMeshComponent)

SkeletalMeshComponent::SkeletalMeshComponent() {}
SkeletalMeshComponent::~SkeletalMeshComponent() {
	Clear();
}

void SkeletalMeshComponent::SkipMesh(unsigned int at_index)
{
	skip_meshs.push_back(at_index);
}

void SkeletalMeshComponent::UnSkipMesh(unsigned int at_index)
{
	for (size_t i = 0; i < skip_meshs.size(); i++)
	{
		if(skip_meshs[i] == at_index)
			skip_meshs.erase(skip_meshs.begin() + i);
	}
}

bool SkeletalMeshComponent::isMeshSkiped(unsigned int at_index)
{
	for (size_t i = 0; i < skip_meshs.size(); i++)
		if (skip_meshs[i] == at_index)
			return true;
	return false;
}

void SkeletalMeshComponent::AddAnimation(Animation * anim)
{
	if (anim == nullptr) return;

	for (size_t i = 0; i < animations.size(); i++)
	{
		if (animations[i]->anim_name == anim->anim_name)
		{
			std::cout << "Animation already loaded!" << std::endl;
			return;
		}
	}
	animations.push_back(anim);
	animator.PlayAnimation(anim);
}
/*
void SkeletalMeshComponent::LoadAnimation(std::string anim_name, std::string anim_path, unsigned int at_index)
{
	for (size_t i = 0; i < animations.size(); i++)
	{
		if (animations[i]->anim_name == anim_name && animations[i]->anim_index == at_index)
		{
			std::cout << "Animation already loaded!" << std::endl;
			return;
		}
	}

	Animation* anim = new Animation;
	anim->anim_name = anim_name;
	if (!anim->LoadAnimation(anim_path, mesh, at_index))
	{
		delete anim;
		return;
	}
	
	animations.push_back(anim);
	animator.PlayAnimation(anim);
}
*/
void SkeletalMeshComponent::RemoveAnimation(std::string anim_name)
{
	for (size_t i = 0; i < animations.size(); i++)
	{
		if (animations[i]->anim_name == anim_name)
		{
			//Animation* anim = animations[i];
			animations.erase(animations.begin() + i);
			//delete anim;
			return;
		}
	}
}

void SkeletalMeshComponent::PlayAnimation(std::string anim_name)
{
	for (size_t i = 0; i < animations.size(); i++)
	{
		if (animations[i]->anim_name == anim_name)
		{
			animator.PlayAnimation(animations[i]);
			return;
		}
	}
}

void SkeletalMeshComponent::Update(float dt)
{
	dt *= speed;
	if (mesh == nullptr)
		return;
	if (animations.empty())
		return;
	if (!animator.HasAnimation())
		return;
	animator.UpdateAnimation(dt, scale);
}

void SkeletalMeshComponent::Clear()
{
	//std::cout << "Removing Skeletal animation component\n";
	for (size_t i = 0; i < animations.size(); i++)
	{
		delete animations[i];
	}
	animations.clear();
}

void SkeletalMeshComponent::Draw()
{
	//mesh->Draw();
	for (unsigned int i = 0; i < mesh->meshes.size(); i++)
	{
		if (isMeshSkiped(i)) continue;
		mesh->meshes[i].Draw();
	}
}

