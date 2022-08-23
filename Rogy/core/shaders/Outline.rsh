
// --------------------------------------------[ Vertex Shader ]------------------------------------------------------

#ifdef COMPILING_VS

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;


out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
} vs_out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
	mat4 VP; // projection * view
};

uniform mat4 model;

void main()
{
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));

	//gl_Position = projection *  view * model * vec4(aPos.xyz + aNormal * 0.06, 1.0);

    gl_Position = VP * model * vec4(aPos, 1.0);
}

#endif // COMPILING_VS

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS
layout (location = 0) out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
} fs_in;

uniform vec4 color;

void main()
{
	FragColor = color;//vec4(0.75, 0.76, 0.39, 1.0);
}

#endif // COMPILING_FS