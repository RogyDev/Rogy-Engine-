#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <unordered_map>

class Shader
{
public:
	int id;
	const char* name;
	GLuint Program;
	static unsigned int gNUM_CASCADES;
	static unsigned int gNUM_LIGHTS;
	static unsigned int gSHADOW_QUALITY;

	std::unordered_map<std::string, GLuint> mUniformsCache;

	Shader();

	~Shader();
	void Shader::setShader_g(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath);
	void Shader::setShader(const GLchar* vertexPath, const GLchar* fragmentPath);
	void Shader::loadShader(const GLchar* ShaderPath, const GLchar* defines = "NON");
	void Shader::loadShaderGeo(const GLchar* ShaderPath);
	void Shader::loadComputeShader(const GLchar* ShaderPath);
	void use();

	void SetMat4(GLuint uniform_id, glm::mat4 value);
	void SetVec3(GLuint uniform_id, glm::vec3 value);
	void SetVec3(GLuint uniform_id, float value_x, float value_y, float value_z);
	void SetVec4(const char* uniform_id, glm::vec4 value);
	void SetVec2(GLuint uniform_id, glm::vec2 value);
	void SetVec2(GLuint uniform_id, float value_x, float value_y);
	void SetFloat(GLuint uniform_id, float value);
	void setBool(GLuint uniform_id, bool value);
	void setInt(GLuint uniform_id, int value);

	void SetMat4(const char* uniform_id, glm::mat4 value);
	void SetVec3(const char* uniform_id, glm::vec3 value);
	void SetVec3(const char* uniform_id, float value_x, float value_y, float value_z);
	void SetVec2(const char* uniform_id, glm::vec2 value);
	void SetVec2(const char* uniform_id, float value_x, float value_y);
	void SetFloat(const char* uniform_id, float value);
	void setBool(const char* uniform_id, bool value);
	void setInt(const char* uniform_id, int value);

	GLuint GetUniform(const std::string &unif_name);
	GLuint GetUniform1(const std::string unif_name);
	GLuint GetUniform2(const char* unif_name) const;

	void deleteShader();
};

#endif
