
// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 0.0, 1.0); 
}  

#endif
// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS

out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D screenTexture;

void main()
{
	FragColor = texture(screenTexture, TexCoords);
}

#endif
