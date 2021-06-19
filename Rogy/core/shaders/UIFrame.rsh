
// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS
layout (location = 0) in vec3 vertex; 
layout (location = 1) in vec2 Texcoords; 

out vec2 TexCoords;
uniform mat4 projection;
uniform mat4 model;

//uniform float aspectRatio;

void main()
{
	TexCoords = -Texcoords;
	//TexCoords = vertex.zw;
	gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
	//TexCoords = vec2((vertex.x+1.0)/2.0, 1 - (vertex.y+1.0)/2.0);
}  

#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS

in vec2 TexCoords;
out vec4 color;

uniform sampler2D Image;
uniform vec2 ss;
uniform vec2 sp;
//uniform vec4 Color;

void main()
{
	//color = texture(Image, /*gl_FragCoord*/TexCoords);
	//color = texture(Image, vec2((gl_FragCoord.x/ss.x) + (ss.x/sp.x), (gl_FragCoord.y/ss.y)));
	color = texture(Image, vec2(((gl_FragCoord.x - sp.x)/ss.x), (gl_FragCoord.y/ss.y)));
	//col = vec4(1.0, 1.0, 1.0, 1.0); //Color;
	//col = vec4(color, 1.0); //Color;
}

#endif