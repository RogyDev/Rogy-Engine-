
// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 WorldPos;


void main()
{
    WorldPos = aPos;
	
	mat4 rotView = mat4(mat3(view));
	vec4 clipPos = projection * rotView * vec4(aPos, 1.0);

	gl_Position = clipPos.xyww;
}
#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 gPosition;

in vec3 WorldPos;

uniform samplerCube environmentMap;

void main()
{		
	FragColor = texture(environmentMap, WorldPos);
	gPosition = vec3(9999999.0, 9999999.0, 9999999.0);
	/*vec3 V = normalize(WorldPos);
	float cos_theta = clamp(V.y, 0, 1);
	float cos_gamma = dot(V, sun_direction);
	float gamma = acos(cos_gamma);

	vec3 Z = params[9];
	vec3 R = Z * HosekWilkie(cos_theta, gamma, cos_gamma);
	if (cos_gamma > 0) {
		// Only positive values of dot product, so we don't end up creating two
		// spots of light 180 degrees apart
		R = R + pow(vec3(cos_gamma), vec3(256)) * 0.5;
	}
	FragColor = vec4(R, 1.0);*/
	
	//FragColor = texture(environmentMap, WorldPos);
	
	//vec3 envColor = texture(environmentMap, WorldPos).rgb;
    
    //FragColor = vec4(envColor, 1.0);
	// HDR tonemap and gamma correct
	//envColor = envColor / (envColor + vec3(1.0));
   // envColor = pow(envColor, vec3(1.0/2.2)); 
}

#endif