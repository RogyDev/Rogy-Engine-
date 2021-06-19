
// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 MVP;

void main()
{
    TexCoords = aPos;
    //gl_Position = MVP  * vec4(aPos, 1.0);
    vec4 pos = MVP * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}
#endif