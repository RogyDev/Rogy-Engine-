
// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
uniform mat4 projection;
uniform mat4 model;

//uniform float aspectRatio;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * model * vec4(aPos, 0.0, 1.0); 
}  

#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS

in vec2 TexCoords;
out vec4 color;

uniform sampler2D Image;
uniform vec3 Color;
uniform float Alpha;
uniform bool use_image;

void main()
{
	vec4 sampled = vec4(1.0);
	if(use_image)
		sampled = texture(Image, TexCoords);
	color = vec4(Color, Alpha) * sampled;
}

#endif