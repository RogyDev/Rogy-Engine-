#ifndef DEFAULT_COMPONENTS_H
#define DEFAULT_COMPONENTS_H

#include "../scene/RComponent.h"
#include <glm/glm.hpp>
#include <string>

class ExampleComponent : public Component
{
	BASE_COMPONENT()
public:

	int properti = 5;
	
};


#endif // DEFAULT_COMPONENTS_H