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


private:

};


#endif // RENDER_CACHE_H