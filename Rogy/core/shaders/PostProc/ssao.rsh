

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

 float kPi = 3.141592653589793238;

uniform vec3 camPos;

uniform vec2 u_resolution;

// tile noise texture over screen based on screen dimensions divided by noise size
uniform mat4 projection;
uniform mat4 view;

/** Reructs screen-space unit normal from screen-space position */
vec3 reructCSFaceNormal(vec3 C) {
	return normalize(cross(dFdy(C), dFdx(C)));
}

vec2 getRandom(vec2 uv) 
{	
	return normalize(texture(texNoise, u_resolution * uv / 10).xy * 2.0f - 1.0f); 
}

float doAmbientOcclusion(vec2 tcoord,vec2 uv, vec3 p, vec3 cnorm) 
{
  vec3 diff = texture(gPosition, (tcoord + uv)).xyz; 
  diff-= p;
  vec3 v = normalize(diff); 
  float d = length(diff)*radius; 
  return max(0.0,dot(cnorm,v)-bias)*(1.0/(1.0+d))*0.5;
}


void main()
{
	// get this pixel position, normal and random noise
    vec3 position = texture(gPosition, TexCoords).xyz;
    //vec3 normal = texture(gNormal, TexCoords).rgb;
	vec3 normal = normalize(cross(dFdx(position), dFdy(position)));

	vec2 randomTexCoordScale = u_resolution;
	randomTexCoordScale /= 4;
    vec3 randomVec = texture(texNoise, TexCoords * randomTexCoordScale).xyz;
	
	//vec2 noiset = getRandom(TexCoords);
	float occlusion = 0.0;  // total summed occlusion

/* /GTAO : https://github.com/krzysztofmarecki/OpenGL/blob/master/OpenGL/src/shaders/gtao.frag
	vec3 vsCenterPos = position;
	vec3 vsV = normalize(-vsCenterPos);
	vec3 vsNormal = normal;
	float rRadius = min(radius / abs(vsCenterPos.z), radius);

	ivec2 xy = ivec2(gl_FragCoord);
	 float radAngle = 0.0 +
		(1.0 / 16.0) * ((((xy.x+xy.y) & 0x3) << 2) + (xy.x & 0x3)) * kPi * 2;

	 vec3 direction = vec3(cos(radAngle), sin(radAngle), 0);
	 vec3 orthoDirection = direction - dot(direction, vsV) * vsV;
	 vec3 vsAxis = cross(direction, vsV);
	 vec3 vsProjectedNormal = vsNormal - dot(vsNormal, vsAxis) * vsAxis;

	 float signN = sign(dot(orthoDirection, vsProjectedNormal));
	 float cosN = clamp(dot(vsProjectedNormal, vsV) / length(vsProjectedNormal), 0, 1);
	 float n = signN * acos(cosN);

	 int kNumDirectionSamples = kernelSize;
	 float ssStep = rRadius / kNumDirectionSamples;
	float ao = 0;
	for (int side = 0; side <= 1; side++) {
		float cosHorizon = -1;
		vec2 uv = TexCoords;
		uv += (-1 + 2 * side) * direction.xy *
			0.25 * ((xy.y - xy.x) & 0x3) * bias;
		for (int i = 0; i < kNumDirectionSamples; i++) {
			uv += (-1 + 2 * side) * direction.xy * (ssStep);
			 vec3 vsSamplePos = texture(gPosition, uv).xyz;
			 vec3 vsHorizonVec = (vsSamplePos - vsCenterPos);
			 float lenHorizonVec = length(vsHorizonVec);
			 float cosHorizonCurrent =  dot(vsHorizonVec, vsV) / lenHorizonVec;
			if (lenHorizonVec < 56.89/4)
				cosHorizon = max(cosHorizon, cosHorizonCurrent);
		}
		 float radHorizon = n + clamp((-1 + 2*side) * acos(cosHorizon) - n, -kPi/2, kPi/2);
		ao += length(vsProjectedNormal) * 0.25 * (cosN + 2 * radHorizon * sin(n) - cos(2 * radHorizon -n));
	}

	occlusion = 1.0 - ao;
*/
	/*

	 vec2 vecc[4] = {vec2(1,0),vec2(-1,0), vec2(0,1),vec2(0,-1)};
	vec3 p = position; 
	vec3 n = normal;

    // SSAO parameters
   
	
	int iterations = kernelSize; 

	for (int j = 0; j < iterations; ++j) 
  	{
		vec2 coord1 = reflect(vecc[j],randomVec.xy)*radius; 
    	vec2 coord2 = vec2(coord1.x*0.707 - coord1.y*0.707, coord1.x*0.707 + coord1.y*0.707); 
    
    	occlusion += doAmbientOcclusion(TexCoords,coord1*0.25, p, n); 
    	occlusion += doAmbientOcclusion(TexCoords,coord2*0.5, p, n); 
    	occlusion += doAmbientOcclusion(TexCoords,coord1*0.75, p, n); 
    	//occlusion += doAmbientOcclusion(TexCoords,coord2, p, n); 
  	}
  
  occlusion/= iterations*3.0; 
*/
	float m_radius = radius; //*length(camPos - position);
	// create tangent space -> view space transformation
	vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN       = mat3(tangent, bitangent, normal);

    // use the whole kernel
	for(int i = 0; i < kernelSize; i++) 
	{
		// extract the kernel sample
		//vec2 kernelCoords = vec2(i/8, i%8);  // make a 
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

	//if (occlusion < 0.05) occlusion = 1.0;
	
    // calculate the ratio
    float occlusionFactor = 1.0 - (occlusion / kernelSize);
    // add power => make occlusion stronger
    occlusionFactor = pow(occlusionFactor, power);
    // return final color
    FragColor = occlusionFactor;
	
}

#endif