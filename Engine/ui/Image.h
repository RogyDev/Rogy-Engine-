#ifndef UI_IMAGE_H
#define UI_IMAGE_H

#include "UITransform.h"
#include "../shading/texture.h"

class UIImage : public UITransform
{
	BASE_COMPONENT()

	glm::vec3 color = glm::vec3(1.0f);
	float alpha = 1.0f;
	float rotation = 0.0f;
	Texture* texture = nullptr;
};


#endif // UI_IMAGE_H
