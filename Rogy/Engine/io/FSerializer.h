#ifndef R_FILE_SERIALIZER_H
#define R_FILE_SERIALIZER_H

#include <iostream>
#include <string>

#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>
#include <glm\glm.hpp>
#include <glm/gtx/quaternion.hpp>

#define SER_VERSION "1.0"

#include <fstream>
#include <yaml-cpp/yaml.h>
/*
YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::quat& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}
*/
void Vec3Bin(glm::vec3, cereal::BinaryOutputArchive& ar);
glm::vec3 Vec3Bin(cereal::BinaryInputArchive& ar);

namespace RYAML{
	void EndSave(YAML::Emitter& out, std::string& path);
	void EndSave(YAML::Emitter& out, const char* path);

	void SerVec2(YAML::Emitter& out, const glm::vec2& v);
	void SerVec3(YAML::Emitter& out, const glm::vec3& v);
	void SerVec4(YAML::Emitter& out, const glm::vec4& v);
	void SerQuat(YAML::Emitter& out, const glm::quat& v);
	glm::vec2 GetVec2(YAML::Node& out);
	glm::vec3 GetVec3(YAML::Node& out);
	glm::vec4 GetVec4(YAML::Node& out);
	glm::quat GetQuat(YAML::Node& out);

}

#endif