#include "billboardComponent.h"

IMPL_COMPONENT(BillboardComponent)

BillboardComponent::BillboardComponent()
{
	color = glm::vec3(1.0f);
	enabled = true;
	sun_source = false;
	use_tex_as_mask = false;
	pos = glm::vec3(0.0f);
	size = glm::vec2(1.0f);
	depth_test = true;
	tex_path = std::string("core\\textures\\glow1.png");
}

BillboardComponent::~BillboardComponent()
{

}
