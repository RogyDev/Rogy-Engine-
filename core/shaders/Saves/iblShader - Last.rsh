
// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec3 WorldPos;
out mat3 TBN;
out vec4 LightSpacePos[3];
out float ClipSpacePosZ;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 gLightWVP[3];

void main()
{
	TexCoords = aTexCoords;
	WorldPos = aPos;
    Normal = mat3(transpose(inverse(model))) * aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
	
	vec4 Pos = vec4(FragPos, 1.0);
	for (int i = 0 ; i < 3 ; i++) 
	{
       LightSpacePos[i] = gLightWVP[i] * Pos;
    }
	
	vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
	vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
	TBN = mat3(T, B, N);

    gl_Position = projection * view * model * vec4(aPos, 1.0);
	
	ClipSpacePosZ = gl_Position.z;
}

#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS

struct SpotLight 
{
    vec3 position;
	vec3 direction;
	vec3 color;
	
	float intensity;
	float cutOff;
	float outerCutOff;
	
    float raduis;
	
	bool active;
};

struct PointLight 
{
    vec3 position;
    vec3 color;
	
	float intensity;
    float raduis;
	
	bool active;
	bool cast_shadows;
	int  shadow_index;
};

struct DirLight 
{
    vec3 direction;
	vec3 color;
	
	float intensity;
	bool active;
	
	bool cast_shadows;
	bool soft_shadows;
};

struct FogEffect 
{
	vec3 color;
	float near;
	float far;
	bool active;
};

#define MAX_LIGHT_COUNT 32
#define NUM_CASCADES 2

out vec4 FragColor;

in vec4 LightSpacePos[3];
in float ClipSpacePosZ;

in vec3 Normal;
in vec3 FragPos;
in vec3 WorldPos;
in vec2 TexCoords;
in mat3 TBN; 

uniform sampler2DShadow shadowMaps[3];
uniform float CascadeEndClipSpace[3];

// material parameters
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

// textures
uniform bool use_tex_albedo;
uniform sampler2D tex_albedo;

uniform bool use_tex_metal;
uniform sampler2D tex_metal;

uniform bool use_tex_rough;
uniform sampler2D tex_rough;

uniform bool use_tex_normal;
uniform sampler2D tex_normal;

// environmet
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;

// ligthing
uniform vec3 CamPos;
uniform PointLight[MAX_LIGHT_COUNT] p_lights;
uniform SpotLight[MAX_LIGHT_COUNT] sp_lights;
uniform DirLight dirLight;

// Point Shadows
uniform samplerCube tex_shadows[8];

// Fog
uniform FogEffect Fog;

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float PointShadowCalculation(vec3 fragPos, vec3 lightPos, float far_plane, int shadow_index)
{
    vec3 fragToLight = fragPos - lightPos;
    float closestDepth = texture(tex_shadows[shadow_index], fragToLight).r;
    closestDepth *= far_plane;
    float currentDepth = length(fragToLight);
    float bias = 0.05; 
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0; 
	
	/*vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);
	 float shadow = 0.0;
    float bias = 0.0;
    int samples = 20;
    float viewDistance = length(CamPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(PointdepthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);*/

	
    return shadow;
}

// -----------------------------------------------------------------------------------------------------------------------
vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

// Returns a random number based on a vec3 and an int.
float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float ShadowCalculation(int sh_index, bool softS)
{
	vec4 fragPosLightSpace = LightSpacePos[sh_index];
	
	float bias = 0.005;

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;
	
	float shadow = 0.0;
	
	if(softS)
	{
		// PCF
		vec2 texelSize = 1.0 / textureSize(shadowMaps[sh_index], 0);
	
		for(int x = -1; x <= 1; ++x)
		{
			for(int y = -1; y <= 1; ++y)
			{		
				shadow += (1.0 - texture( shadowMaps[sh_index], vec3(projCoords.xy + vec2(x, y) * texelSize, projCoords.z - bias) ));
			}    
		}
	
		shadow /= 9.0;
	}
	else
	{
		for (int i = 0; i < 2 ; i++)
		{
			int index = i*2;
			//int index = int(16.0*random(FragPos.xyy*1000.0, i))%16;
			//int index = int(16.0*random(floor(WorldPos.xyz*10000.0), i))%16;
			
			shadow += (1.0 - texture( shadowMaps[sh_index], vec3(projCoords.xy + poissonDisk[index]/700.0, projCoords.z - bias) ));
		}
    }
	
	if(shadow > 1)
		shadow = 1;
		
    return shadow;
}

// -----------------------------------------------------------------------------------------------------------------------
const float PI = 3.14159265359;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(tex_normal, TexCoords).xyz * 2.0 - 1.0;
	return normalize(TBN * tangentNormal);
}
// -----------------------------------------------------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float a)
{
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}
// -----------------------------------------------------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// -----------------------------------------------------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// -----------------------------------------------------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}  
// -----------------------------------------------------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// -----------------------------------------------------------------------------------------------------------------------
vec3 CalcIBL(vec3 N, vec3 V, vec3 F0, vec3 albedo_color, float metallic_color, float roughness_color)
{
    vec3 R = reflect(-V, N);
	
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness_color);
    
    vec3 kD = (1.0 - F) * (1.0 - metallic_color);
    
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse    = irradiance * albedo_color;
    
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness_color * 4).rgb;      
    vec3 specular = prefilteredColor * F;
    
	return  (kD * diffuse + specular) * ao;
}
// -----------------------------------------------------------------------------------------------------------------------
vec3 CalcDirLight(DirLight light, vec3 V, vec3 N, vec3 F0, vec3 albedo_color, float metallic_color, float roughness_color)
{
	float shadow = 0;
    
	if(light.cast_shadows)
	{
		for (int i = 0 ; i < NUM_CASCADES ; i++) 
		{
			if (ClipSpacePosZ <= CascadeEndClipSpace[i])
			{
				shadow = ShadowCalculation(i, light.soft_shadows);
				break;
			}
		}
    }
	
	if(shadow == 1) return vec3(0); // Don't calculate lighting inside the shadow
	
	vec3 L = normalize(light.direction);
    vec3 H = normalize(V + L);
	
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness_color);   
    float G   = GeometrySmith(N, V, L, roughness_color);      
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);
	
    vec3 nominator    = NDF * G * F; 
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = nominator / max(denominator, 0.001);

    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - metallic_color;	  
    float NdotL = max(dot(N, L), 0.0);        
	
	return ((1.0 - shadow) * (kD * albedo_color / PI + specular) * vec3(1.0) * NdotL) * light.intensity * 5 * light.color;
}
// -----------------------------------------------------------------------------------------------------------------------
vec3 CalcPointLight(PointLight light, vec3 V, vec3 N, vec3 F0, vec3 albedo_color, float metallic_color, float roughness_color)
{
	float shadow = 0.0;//light.cast_shadows ? PointShadowCalculation(FragPos, light.position, 25) : 0.0;                      
	if(light.cast_shadows && light.shadow_index >= 0)
	{
		shadow = PointShadowCalculation(FragPos, light.position, light.raduis, light.shadow_index);
		if(shadow == 1) return vec3(0);
	}
    float distance = length(light.position - FragPos);

	if(distance > light.raduis) return vec3(0);
	
	vec3 L = normalize(light.position - FragPos);
    vec3 H = normalize(V + L);
	
	//float attenuation = 1 * ((distance + light.quadratic/light.linear) * (distance * distance)) / (light.linear * distance);

    float attenuation = 1.0 - (distance / (light.raduis));

	if(attenuation < 0) attenuation = 0;
	if(attenuation > 1) attenuation = 1;
	
    vec3 radiance = vec3(1.0) * attenuation * light.intensity;
	
    // Cook-Torrance BRDF direction
    float NDF = DistributionGGX(N, H, roughness_color);   
    float G   = GeometrySmith(N, V, L, roughness_color);      
    vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
    
    vec3 nominator    = NDF * G * F; 
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = nominator / max(denominator, 0.001);

    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - metallic_color;	  
    float NdotL = max(dot(N, L), 0.0);        
	
	return ((1.0 - shadow) * ( kD * albedo_color / PI + specular)) * radiance * NdotL * light.color;
}
// -----------------------------------------------------------------------------------------------------------------------
vec3 CalcSpotLight(SpotLight light, vec3 V, vec3 N, vec3 F0, vec3 albedo_color, float metallic_color, float roughness_color)
{
    float distance = length(light.position - FragPos);
	if(distance > light.raduis) return vec3(0);
	
	vec3 L = normalize(light.position - FragPos);
	float theta = dot(L, normalize(-light.direction));
	
	if(theta < light.outerCutOff)  return vec3(0); // remember that we're working with angles as cosines instead of degrees so a '>' is used.
	
	// spotlight (soft edges)
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensitys = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	vec3 H = normalize(V + L);

    float attenuation = 1.0 - (distance / (light.raduis));
	if(attenuation < 0) attenuation = 0;
	if(attenuation > 1) attenuation = 1;
	
    vec3 radiance = vec3(1.0) * attenuation * light.intensity * intensitys;
	
    // Cook-Torrance BRDF direction
    float NDF = DistributionGGX(N, H, roughness_color);   
    float G   = GeometrySmith(N, V, L, roughness_color);      
    vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
    
    vec3 nominator    = NDF * G * F; 
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = nominator / max(denominator, 0.001);

    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - metallic_color;	  
    float NdotL = max(dot(N, L), 0.0);        
	
	return (kD * albedo_color / PI + specular) * radiance * NdotL * light.color;
}
// -----------------------------------------------------------------------------------------------------------------------
vec3 CalcFog()
{	
	vec3 fg = vec3(0.0);
	float distance = length(CamPos - FragPos);
	if(distance < Fog.near) return vec3(0);
	fg = Fog.color;
	
	float attenuation = ((distance - Fog.near) / (Fog.far - Fog.near));

	if(attenuation < 0) attenuation = 0;
	if(attenuation > 1) attenuation = 1;
	
	fg *= attenuation;
	return fg;
}
// -----------------------------------------------------------------------------------------------------------------------
void main()
{   
    // Main Colors
	//--------------------------------------------
    vec3  albedo_color     = albedo;
    float metallic_color   = metallic;
    float roughness_color  = roughness;
	
	vec3 N; 									// Normal
	vec3 V = normalize(CamPos - FragPos);	    // View Direction
	
	if(use_tex_albedo)
    albedo_color *= pow(texture(tex_albedo, TexCoords).rgb, vec3(2.2));
	
	if(use_tex_metal)
    metallic_color *= texture(tex_metal, TexCoords).r;
	
	if(use_tex_rough)
    roughness_color *= texture(tex_rough, TexCoords).r;
	
	if(use_tex_normal)
	N = getNormalFromMap();
	else
	N = normalize(Normal);
	
	// Ambient Lighting (IBL)
	//--------------------------------------------
	vec3 F0 = mix(vec3(0.04), albedo_color, metallic_color); 
    vec3 ambient = CalcIBL(N, V, F0, albedo_color, metallic_color, roughness_color);
	
	// Direct Lighting
	//--------------------------------------------
    vec3 Lo = vec3(0.0);
	
	if(dirLight.active) Lo += CalcDirLight(dirLight, V, N, F0, albedo_color, metallic_color, roughness_color);
	
	//vec3 Lo2 = vec3(0.0);
	
	for(int i = 0; i < MAX_LIGHT_COUNT; i++)
	{
		if(p_lights[i].active)
				Lo += CalcPointLight(p_lights[i], V, N, F0, albedo_color, metallic_color, roughness_color);
		else
			break;
	}
	
	for(int i = 0; i < MAX_LIGHT_COUNT; i++)
	{
		if(sp_lights[i].active)
				Lo += CalcSpotLight(sp_lights[i], V, N, F0, albedo_color, metallic_color, roughness_color);
		else
			break;
	}
   
    /* if(use_shadows)
    {
		vec2 ScrFragPos = (gl_FragCoord.xy / u_resolution);
		float shd   = texture(tex_shadows, ScrFragPos).r;
		Lo2 *= shd;
	}
	Lo += Lo2;*/
	
	
	// Final Shading
	//--------------------------------------------
	vec3 color = ambient + Lo;
	
	// apply fog
	//color += CalcFog();
	
	// HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));
	
    FragColor = vec4(color , 1.0);// + CascadeIndicator;
}

#endif