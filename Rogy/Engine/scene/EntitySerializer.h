#ifndef R_ENTITY_SERIALIZER_H
#define R_ENTITY_SERIALIZER_H

#include <iostream>
#include <string>

#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>

#include "../renderer/rendererComponent.h"
#include "../renderer/lighting.h"
#include "../renderer/reflection_probe.h"
#include "../renderer/billboardComponent.h"
#include "../renderer/cameraComponent.h"
#include "../renderer/ParticleSystem.h"
#include "../audio/AudioSource.h"
#include "../renderer/GrassComponent.h"
#include "../ui/UIWidget.h"
#include "../animation/SkeletalMeshComponent.h"
#include "../scene/Entity.h"

namespace EntitySerializer
{

}

#endif // R_ENTITY_SERIALIZER_H