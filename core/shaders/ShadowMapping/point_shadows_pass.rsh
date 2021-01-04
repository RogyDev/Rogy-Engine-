// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	TexCoords = aTexCoords;
    Normal = mat3(transpose(inverse(model))) * aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
	
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS

//#define MAX_LIGHT_COUNT 1

//layout (location = 0) out vec3 FragColor;
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct PointLight 
{
    vec3   position;
	float  intensity;
    float  raduis;
	bool   active;
	samplerCube shadowMap;
};

uniform PointLight[8] plight;

float PointShadowCalculation(vec3 fragPos, vec3 lightPos, float far_plane, int sh_index)
{
    vec3 fragToLight = fragPos - lightPos;
    float closestDepth = texture(plight[sh_index].shadowMap, fragToLight).r;
    closestDepth *= far_plane;
    float currentDepth = length(fragToLight);
    float bias = 0.00; 
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0; 

    return shadow;
}

vec3 CalcPointLight(PointLight light, vec3 norm, int sh_index)
{
	float distance = length(light.position - FragPos);
	if(distance > light.raduis) return vec3(1);
	
	float shadow = 0.0;                   
	shadow = PointShadowCalculation(FragPos, light.position, light.raduis, sh_index);
	if(shadow == 1) return vec3(0);
	
	return (1.0 - shadow) * vec3(1);
}

void main()
{
	vec3 ambient = vec3(0.0);
	
	vec3 N = normalize(Normal);
	
	vec3 Lo = vec3(0.0);
	for(int i = 0; i < 1; i++)
	{
		if(plight[i].active)
				Lo += CalcPointLight(plight[i], N, i);
		else
			break;
	}
	FragColor = vec4(Lo, 1.0);
}
#endif
