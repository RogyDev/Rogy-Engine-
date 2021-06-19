#include "FSerializer.h"

void Vec3Bin(glm::vec3 t, cereal::BinaryOutputArchive& ar)
{	
	ar(t.x);
	ar(t.y);
	ar(t.z);
}
glm::vec3 Vec3Bin(cereal::BinaryInputArchive& ar)
{	
	glm::vec3 t;
	ar(t.x);
	ar(t.y);
	ar(t.z);
	return t;
}

namespace RYAML {
	void EndSave(YAML::Emitter& out, std::string& path)
	{
		std::ofstream fout(path);
		fout << out.c_str();
		fout.close();
	}
	// ------------------------------------------------------------
	void EndSave(YAML::Emitter& out, const char* path)
	{
		std::ofstream fout(path);
		fout << out.c_str();
		fout.close();
	}
	// ------------------------------------------------------------
	void SerVec2(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
	}
	// ------------------------------------------------------------
	void SerVec3(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	}
	// ------------------------------------------------------------
	void SerVec4(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	}
	// ------------------------------------------------------------
	void SerQuat(YAML::Emitter& out, const glm::quat& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	}
	// ------------------------------------------------------------
	glm::vec2 GetVec2(YAML::Node& out)
	{
		glm::vec2 v;
		if (out.IsSequence()) {
			v.x = out[0].as<float>();
			v.y = out[1].as<float>();
		}
		return v;
	}
	// ------------------------------------------------------------
	glm::vec3 GetVec3(YAML::Node& out)
	{
		glm::vec3 v;
		if (out.IsSequence()) {
			v.x = out[0].as<float>();
			v.y = out[1].as<float>();
			v.z = out[2].as<float>();
		}
		return v;
	}
	// ------------------------------------------------------------
	glm::vec4 GetVec4(YAML::Node& out)
	{
		glm::vec4 v;
		if (out.IsSequence()) {
			v.x = out[0].as<float>();
			v.y = out[1].as<float>();
			v.z = out[2].as<float>();
			v.w = out[3].as<float>();
		}
		return v;
	}
	// ------------------------------------------------------------
	glm::quat GetQuat(YAML::Node& out)
	{
		glm::quat v;
		if (out.IsSequence()) {
			v.x = out[0].as<float>();
			v.y = out[1].as<float>();
			v.z = out[2].as<float>();
			v.w = out[3].as<float>();
		}
		return v;
	}
}