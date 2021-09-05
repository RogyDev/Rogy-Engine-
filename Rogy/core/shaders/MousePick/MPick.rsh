

// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS
layout (location = 0) in vec3 aPos;

// Uniforms
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 FragPos;

void main() 
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS
//out int FragColor;
out vec4 FragColor;

in vec3 FragPos;

// parameters
uniform int entityID;

void main()
{
    //FragColor = entityID;
	FragColor = vec4(FragPos, entityID);
}

#endif