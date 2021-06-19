#ifndef SK_ANIMATION_STATE_H
#define SK_ANIMATION_STATE_H

#include "Animation.h"

struct AnimState
{
	std::string name;
	Animation* anim;
	float value;

	AnimState(std::string aName, Animation* aAnim, float aValue = 1.0f)
	{
		name = aName;
		anim = aAnim;
		value = aValue;
	}
};

class AnimBlendState
{
public:
	std::vector<AnimState> anims;

	void AddAnimState(std::string name, Animation* anim, float value = 1.0f)
	{
		anims.emplace_back(name, anim, value);
	}

	void SetAnimValue(std::string name, float val)
	{
		for (size_t i = 0; i < anims.size(); i++)
		{
			if (anims[i].name == name)
				anims[i].value = val;
		}
	}
};

#endif // ! SK_ANIMATION_STATE_H