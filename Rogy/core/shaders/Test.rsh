
// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
  	gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#endif // COMPILING_VS

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 gPosition;

void main()
{
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}

#endif // COMPILING_FS