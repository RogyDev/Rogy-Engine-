
// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat4 u_model;

out vec2 v_texcoord;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(aPos, 1.0);
	//gl_Position = u_projection * (u_view * vec4(aPos, 1.0));
	v_texcoord = aTexCoords;
}
#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS

out vec4 FragColor;

in vec2 v_texcoord;
uniform sampler2D u_lightmap;

void main()
{
	FragColor = vec4(texture(u_lightmap, v_texcoord).rgb, gl_FrontFacing ? 1.0 : 0.0);
	//FragColor = vec4(texture(u_lightmap, v_texcoord).rgb, 1.0);
}

#endif