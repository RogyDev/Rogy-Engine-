
// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS
//layout(location = 0) in vec3 squareVertices;

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 UV;

uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 VP; 
uniform vec3 BillboardPos; 
uniform vec2 BillboardSize;

void main()
{
	vec3 particleCenter_wordspace = BillboardPos;
	
	vec3 vertexPosition_worldspace = particleCenter_wordspace 
	+ CameraRight_worldspace * aPos.x * BillboardSize.x 
	+ CameraUp_worldspace * aPos.y * BillboardSize.y;


	gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);

	UV = aTexCoords;//aPos + vec2(0.5, 0.5);
}

#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS
in vec2 UV;
out vec4 color;

uniform sampler2D BBTexture;
uniform vec3 Tex_color;
uniform bool use_tex_as_mask;

void main()
{
	vec4 tex = texture( BBTexture, UV );
	
	if(use_tex_as_mask)
	{	color = vec4(Tex_color, tex.a); }
	else
	{
		vec3 col = tex.rgb * Tex_color;
		color = vec4(col, tex.a);
	}
}
#endif