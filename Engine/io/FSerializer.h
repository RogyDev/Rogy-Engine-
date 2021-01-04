#ifndef R_FILE_SERIALIZER_H
#define R_FILE_SERIALIZER_H

#include <iostream>
#include <string>

#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>
#include <glm\glm.hpp>

#define SER_VERSION "1.0"

void Vec3Bin(glm::vec3, cereal::BinaryOutputArchive& ar);
glm::vec3 Vec3Bin(cereal::BinaryInputArchive& ar);

#endif