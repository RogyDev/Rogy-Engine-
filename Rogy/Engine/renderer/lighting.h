#ifndef R_LIGHTING_H
#define R_LIGHTING_H

#include "light_types\directional_light.h"
#include "light_types\point_light.h"
#include "light_types\spot_light.h"

enum RLightType
{
	R_POINT_LIGHT,
	R_DIRECTIONAL_LIGHT,
	R_SPOT_LIGHT
};
#endif