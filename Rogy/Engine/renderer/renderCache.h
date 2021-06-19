#ifndef RENDER_CACHE_H
#define RENDER_CACHE_H

#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <glm/glm.hpp>
#include "../camera/Camera.h"
#include "../resources/material_library.h"

class RCache
{
public:
	RCache();
	~RCache();

	Shader* pbrShader;

	glm::mat4 last_proj;
	glm::mat4 last_view;
	glm::vec3 last_CamPos;

	GLuint irra;
	GLuint pref;
	
	void SetCamera(Camera& cam);
	void BindTexture(ShaderTextureType type, GLuint tex_id);

	GLuint m_albedo, m_norm, m_emm, m_metal, m_rough;
	void BindTexAlbedo(GLuint tex_id);
	void BindTexMetal(GLuint tex_id);
	void BindTexRough(GLuint tex_id);
	void BindTexEmmi(GLuint tex_id);
	void BindTexNormal(GLuint tex_id);


private:

};


#endif // RENDER_CACHE_H