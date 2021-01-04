#ifndef UI_TEXT_H
#define UI_TEXT_H

#include "UITransform.h"

class UIText : public UITransform
{
	BASE_COMPONENT()

	std::string text;
	glm::vec3 color = glm::vec3(1.0f);
	float alpha = 1.0f;
};


#endif
