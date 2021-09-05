// -------------------------------------- Vertex --------------------------------------------------------------------//
#ifdef COMPILING_VS
//#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out VERTEX_OUT {
	vec3 fragmentPosition;
	vec2 textureCoordinates;
	mat3 TBN;
	vec3 tangentViewPosition;
	vec3 tangentFragmentPosition;
	vec3 Normal;
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
	
	vertex_out.Normal = mat3(transpose(inverse(model))) * normal;
}
#endif
// -------------------------------------- Frag --------------------------------------------------------------------//
#ifdef COMPILING_FS
//#version 430
in VERTEX_OUT{
	vec3 fragmentPosition;
	vec2 textureCoordinates;
	mat3 TBN;
	vec3 tangentViewPosition;
	vec3 tangentFragmentPosition;
	vec3 Normal;
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
layout(std430, binding = 0)  buffer LightBuffer {
	PointLight data[];
} lightBuffer;


layout(std430, binding = 1)  buffer VisibleLightIndicesBuffer {
	VisibleIndex data[];
} visibleLightIndicesBuffer;

// Uniforms
//uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;
//uniform sampler2D texture_normal1;
uniform int numberOfTilesX;

out vec4 fragColor;

// Attenuate the point light intensity
float attenuate(vec3 lightDirection, float radius) {
	float cutoff = 0.5;
	float attenuation = dot(lightDirection, lightDirection) / (100.0 * radius);
	attenuation = 1.0 / (attenuation * 15.0 + 1.0);
	attenuation = (attenuation - cutoff) / (1.0 - cutoff);

	return clamp(attenuation, 0.0, 1.0);
}

void main() {
	
	// Determine which tile this pixel belongs to
	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(16, 16);
	int index = tileID.y * numberOfTilesX + tileID.x;

	// Get color and normal components from texture maps
	vec4 base_diffuse = vec4(1.0, 1.0, 1.0, 1.0);//texture(texture_diffuse1, fragment_in.textureCoordinates);
	vec4 base_specular = vec4(1.0, 1.0, 1.0, 1.0);//texture(texture_specular1, fragment_in.textureCoordinates);
	vec3 normal = vec3(1.0, 1.0, 1.0);//texture(texture_normal1, fragment_in.textureCoordinates).rgb;
	normal = normalize(fragment_in.Normal);
	//normal = normalize(normal * 2.0 - 1.0);
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

	vec3 viewDirection = normalize(fragment_in.tangentViewPosition - fragment_in.tangentFragmentPosition);

	// The offset is this tile's position in the global array of valid light indices.
	// Loop through all these indices until we hit max number of lights or the end (indicated by an index of -1)
	// Calculate the lighting contribution from each visible point light
	int offset = index * 1024;
	for (int i = 0; i < 1024 && visibleLightIndicesBuffer.data[offset + i].index != -1; i++) {
		int lightIndex = visibleLightIndicesBuffer.data[offset + i].index;
		PointLight light = lightBuffer.data[lightIndex];

		vec3 lightColor = light.color.xyz;
		vec3 tangentLightPosition = fragment_in.TBN * light.position.xyz;
		float lightRadius = light.paddingAndRadius.w;

		// Calculate the light attenuation on the pre-normalized lightDirection
		vec3 lightDirection = tangentLightPosition - fragment_in.tangentFragmentPosition;
		float attenuation = attenuate(lightDirection, lightRadius);

		// Normalize the light direction and calculate the halfway vector
		lightDirection = normalize(lightDirection);
		vec3 halfway = normalize(lightDirection + viewDirection);

		// Calculate the diffuse and specular components of the irradiance, then irradiance, and accumulate onto color
		float diffuse = max(dot(lightDirection, normal), 0.0);
		// How do I change the material propery for the spec exponent? is it the alpha of the spec texture?
		float specular = pow(max(dot(normal, halfway), 0.0), 32.0);

		// Hacky fix to handle issue where specular light still effects scene once point light has passed into an object
		if (diffuse == 0.0) {
			specular = 0.0;
		}

		vec3 irradiance = lightColor.rgb * ((base_diffuse.rgb * diffuse) + (base_specular.rgb * vec3(specular))) * attenuation;
		color.rgb += irradiance;
	}

	color.rgb += base_diffuse.rgb * 0.08;

	// Use the mask to discard any fragments that are transparent
	if (base_diffuse.a <= 0.2) {
		discard;
	}
	
	fragColor = color;
}
#endif