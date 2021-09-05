// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

// Uniforms
uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main() 
{
	TexCoords = aTexCoords;
	gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}
#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS
//layout(location = 0) out float fragmentdepth;
in vec2 TexCoords;
uniform	sampler2D alpha;
uniform bool use_alpha;
void main()
{
	if(use_alpha)
	{
		vec3 tr = texture(alpha, TexCoords).rgb;
	 	if((tr.r) < 0.5)
       		discard;
	}
	//fragmentdepth = gl_FragCoord.z;
}

#endif