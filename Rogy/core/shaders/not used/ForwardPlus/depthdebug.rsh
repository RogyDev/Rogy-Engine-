
// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;

void main() {
	gl_Position = projection * view * model * vec4(position, 1.0);
	TexCoords = texCoords;
}
#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS

uniform float near;
uniform float far;

out vec4 fragColor;

// Need to linearize the depth because we are using the projection
float LinearizeDepth(float depth) {
	float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
	float depth = LinearizeDepth(gl_FragCoord.z) / far;
	fragColor = vec4(vec3(depth), 1.0f);
}

#endif