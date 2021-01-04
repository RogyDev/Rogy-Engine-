
// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS

layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec4 xyzs; // Position of the center of the particule and size of the square
layout(location = 3) in vec4 color; // Position of the center of the particule and size of the square
layout(location = 4) in vec2 animOffset;

out vec2 UV;
out vec4 particlecolor;

uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 VP; 

uniform vec2 animSheetSize;
uniform bool use_anim;

void main()
{
	float particleSize = xyzs.w; // because we encoded it this way.
	vec3 particleCenter_wordspace = xyzs.xyz;

	vec3 vertexPosition_worldspace = particleCenter_wordspace
	+ CameraRight_worldspace * squareVertices.x * particleSize
	+ CameraUp_worldspace * squareVertices.y * particleSize;

	// Output position of the vertex
	gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);
	particlecolor = color;
	// UV of the vertex. No special space for this one.
	//UV = squareVertices.xy + vec2(0.5, 0.5);
	UV = aTexCoords;
	
	if(use_anim)
	{
		UV.x /= animSheetSize.x; UV.y /= animSheetSize.y;
		UV.x += animOffset.x;
		UV.y -= animOffset.y;
	}
}
#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS
in vec2 UV;
in vec4 particlecolor;

out vec4 color;

uniform sampler2D myTextureSampler;

void main()
{
	color = texture( myTextureSampler, UV ) * particlecolor;
}
#endif