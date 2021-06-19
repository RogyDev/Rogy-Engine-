#ifndef UI_TRANSFORM_H
#define UI_TRANSFORM_H

#include <string>
#include <glm\glm.hpp>
#include "../scene/RComponent.h"


class UITransform : public Component
{
public:
	glm::vec2 Position;
	glm::vec2 Scale;
};


#endif
