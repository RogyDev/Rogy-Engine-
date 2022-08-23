// -------------------------------------- Vertex --------------------------------------------------------------------//
#define MAX_LIGHTS_PER_TILE 64

#ifdef COMPILING_VS
//#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

out VERTEX_OUT{
	vec3 fragmentPosition;
	vec2 textureCoordinates;
	mat3 TBN;
	vec3 tangentViewPosition;
	vec3 tangentFragmentPosition;
} vertex_out;

// Uniforms
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 viewPosition;

void main() {
	gl_Position = projection * view * model * vec4(position, 1.0);
	vertex_out.fragmentPosition = vec3(model * vec4(position, 1.0));
	vertex_out.textureCoordinates = texCoords;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 tan = normalize(normalMatrix * tangent);
	vec3 bitan = normalize(normalMatrix * bitangent);
	vec3 norm = normalize(normalMatrix * normal);

	// For tangent space normal mapping
	mat3 TBN = transpose(mat3(tan, bitan, norm));
	vertex_out.tangentViewPosition = TBN * viewPosition;
	vertex_out.tangentFragmentPosition = TBN * vertex_out.fragmentPosition;
	vertex_out.TBN = TBN;
}

#endif
// -------------------------------------- Frag --------------------------------------------------------------------//
#ifdef COMPILING_FS
#extension GL_ARB_shader_storage_buffer_object : enable
//#version 430
in VERTEX_OUT{
	vec3 fragmentPosition;
	vec2 textureCoordinates;
	mat3 TBN;
	vec3 tangentViewPosition;
	vec3 tangentFragmentPosition;
} fragment_in;

struct PointLight {
	vec4 color;
	vec4 position;
	vec4 paddingAndRadius;
};

struct VisibleIndex {
	int index;
};

// Shader storage buffer objects
layout(std430, binding = 0) readonly buffer LightBuffer{
	PointLight data[];
} lightBuffer;

layout(std430, binding = 1) readonly buffer VisibleLightIndicesBuffer{
	VisibleIndex data[];
} visibleLightIndicesBuffer;

uniform int numberOfTilesX;
uniform int totalLightCount;

out vec4 fragColor;

void main() {
	// Determine which tile this pixel belongs to
	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(16, 16);
	uint index = tileID.y * numberOfTilesX + tileID.x;

	uint offset = index * MAX_LIGHTS_PER_TILE;
	uint i;
	for (i = 0; i < MAX_LIGHTS_PER_TILE && visibleLightIndicesBuffer.data[offset + i].index != -1; i++);

	float ratio = float(i) / float(3);
	fragColor = vec4(vec3(ratio, ratio, ratio), 1.0);
}
#endif