
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

uniform vec2 u_resolution;

// tile noise texture over screen based on screen dimensions divided by noise size
uniform mat4 projection;

/** Reconstructs screen-space unit normal from screen-space position */
vec3 reconstructCSFaceNormal(vec3 C) {
	return normalize(cross(dFdy(C), dFdx(C)));
}

void main()
{
	//ivec2 ssC = ivec2(gl_FragCoord.xy);

	//vec3 fragPos = (view * vec4(positionMetallic.xyz, 1.0)).xyz;
    //vec3 normal  = mat3(view) * normalRoughness.xyz;
	
    // get input for SSAO algorithm
    vec3 fragPos = texture(gPosition, TexCoords).xyz;
	/*if(fragPos == vec3(-1, -1, -1))
	{
		FragColor = vec3(0,0,0);
		return;
	}*/
	
    vec3 normal  =  normalize(texture(gNormal, TexCoords).rgb);
    vec3 randomVec = normalize(texture(texNoise, TexCoords * vec2(u_resolution.x*2/1.0, u_resolution.y*2/1.0)).xyz);
	
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
	
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
	
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 sampl = TBN * samples[i]; // from tangent to view-space
        sampl = fragPos + sampl * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(sampl, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
		
        // get sample depth
        float sampleDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample
		
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		//float rangeCheck= abs(fragPos.z - sampleDepth) < 1.0 ? 1.0 : 0.0;
        occlusion += (sampleDepth >= sampl.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / power);
    
    FragColor = occlusion;
	
}

#endif