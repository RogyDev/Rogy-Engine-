
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
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;

const vec4 skytop = vec4(0.1f, 0.2f, 0.35f, 1.0f);
const vec4 skyhorizon = vec4(0.2f, 0.25f, 0.4f, 1.0f);
 
void main()
{		
	//FragColor = texture(environmentMap, WorldPos);
	
	//vec3 envColor = texture(environmentMap, WorldPos).rgb;
    
    //FragColor = vec4(envColor, 1.0);
	
	vec3 pointOnSphere = normalize(WorldPos);
    float a = pointOnSphere.y * 0.5;
	if(a < 0.0)
	{
		FragColor = vec4(0.05, 0.05, 0.05, 1.0);
		return;
	}
	
	vec3 envColor = mix(skyhorizon, skytop, a).xyz;
	 FragColor = vec4(envColor, 1.0);
	// HDR tonemap and gamma correct
	//envColor = envColor / (envColor + vec3(1.0));
   // envColor = pow(envColor, vec3(1.0/2.2)); 
}

#endif