

// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}
#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS

#define TWO_PI 6.283185307179586476925286766559

out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

// parameters
uniform int kernelSize;
uniform float radius;
uniform float bias;
uniform float power;

uniform vec3 camPos;

uniform vec2 u_resolution;

// tile noise texture over screen based on screen dimensions divided by noise size
uniform mat4 projection;
uniform mat4 view;

/** Reconstructs screen-space unit normal from screen-space position */
vec3 reconstructCSFaceNormal(vec3 C) {
	return normalize(cross(dFdy(C), dFdx(C)));
}

void main()
{
	// get this pixel position, normal and random noise
    vec3 position = texture(gPosition, TexCoords).xyz;

	float m_radius = length(camPos - position) * radius;

    //vec3 normal = texture(gNormal, TexCoords).rgb;
	vec3 normal = normalize(cross(dFdx(position), dFdy(position)));

	vec2 randomTexCoordScale = u_resolution;
	randomTexCoordScale /= 4;
    vec3 randomVec = texture(texNoise, TexCoords * randomTexCoordScale).xyz;
	
    // create tangent space -> view space transformation
    vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN       = mat3(tangent, bitangent, normal);

    // SSAO parameters
    float occlusion = 0.0;  // total summed occlusion
	
    // use the whole kernel
	for(int i = 0; i < kernelSize; i++) 
	{
		// extract the kernel sample
		//vec2 kernelCoords = vec2(i/8, i%8);  // make a const
		//vec3 tsSample = texture(kernel, kernelCoords).xyz;  // tangent space
		//vec3 sampl = TBN * samples[i]; // from tangent to view-space
		vec3 sampl = TBN * samples[i];  // transform sample to view space
		sampl = position + sampl * m_radius; // find the pixel to sample
			
			
		vec4 clipSpacePos = projection * vec4(sampl, 1.0); // from view to clip-space
		vec3 ndcSpacePos = clipSpacePos.xyz /= clipSpacePos.w; // perspective divide
		vec2 windowSpacePos = ((ndcSpacePos.xy + 1.0) / 2.0) * u_resolution;

		if ((windowSpacePos.y > 0) && (windowSpacePos.y < u_resolution.y))
		if ((windowSpacePos.x > 0) && (windowSpacePos.x < u_resolution.x))
		// THEN APPLY AMBIENT OCCLUSION
		{
			// project sample to clip space
			vec4 offset = vec4(sampl, 1.0);
			offset      = projection * offset;  // from view to clip-space
			offset.xyz /= offset.w;  // perspective divide
			offset.xyz  = offset.xyz * 0.5 + 0.5;  // transform to range 0.0 - 1.0

			// finally, sample depth from position
			float sampleDepth = texture(gPosition, offset.xy).z;
			// if they are too far we should ignore this
			float ratio = m_radius / abs(position.z - sampleDepth);
			float rangeCheck = smoothstep(0.0, 7.0, ratio);
			// we moved towards light in a hemisphere and we know
			// our view space z (sample.z), if there is something
			// 'in' from of us (sampleDepth), add occlusion
			if (sampleDepth >= sampl.z + bias) {
				occlusion += 1.0 * rangeCheck;
			}        	
		}
	}
	if (occlusion < 0.05) occlusion = 1.0;
	
    // calculate the ratio
    float occlusionFactor = 1.0 - (occlusion / kernelSize);
    // add power => make occlusion stronger
    occlusionFactor = pow(occlusionFactor, power);
    // return final color
    FragColor = occlusionFactor;
	
}

#endif