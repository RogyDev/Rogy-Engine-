
// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
//#define NUM_CASCADES 2
//#define MAX_LIGHT_COUNT 50

#ifdef COMPILING_VS

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in vec2 aTexCoords2;

#ifdef DISPLACEMENT
layout (location = 6) in float aBlend;
#endif

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
	vec2 TexCoords2;
	vec3 WorldPos;
	mat3 TBN;
	vec4 LightSpacePos[NUM_CASCADES];
	float ClipSpacePosZ;
	vec4 spot_MVP_SPACE[8];
	vec3 FragPosS;
#ifdef DISPLACEMENT
	float Blend;
#endif&
} vs_out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
	mat4 VP; // projection * view
};

uniform mat4 models[1000];
uniform vec2 tex_uv;
uniform mat4 gLightWVP[NUM_CASCADES];
uniform mat4 spot_MVP[8];

void main()
{
#ifdef DISPLACEMENT
	vs_out.Blend = aBlend;
#endif

	mat4 model = models[gl_InstanceID];
	vs_out.TexCoords = aTexCoords * tex_uv;
	vs_out.TexCoords2 = aTexCoords2;
	
	vs_out.WorldPos = aPos;
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));

	vec4 viewPos = view * model * vec4(aPos, 1.0);

	vs_out.FragPosS = viewPos.xyz;
	
	vec4 Pos = vec4(vs_out.FragPos, 1.0);
	for (int i = 0 ; i < NUM_CASCADES ; i++) 
	{
       vs_out.LightSpacePos[i] = gLightWVP[i] * Pos;
    }
	
	for(int i = 0 ; i < 8 ; i++)
	{
		vs_out.spot_MVP_SPACE[i] = spot_MVP[i] * Pos;
	}
	
	vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
	vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
	vs_out.TBN = mat3(T, B, N);

    gl_Position = VP * model * vec4(aPos, 1.0);
	vs_out.ClipSpacePosZ = gl_Position.z;
}

#endif // COMPILING_VS

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS
//#extension GL_ARB_shader_storage_buffer_object : enable
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 gPosition;

// -------------- STRUCTS -------------- //

struct DirLight 
{
	bool use;
    vec3 direction;
	vec3 color;
	float intensity;
	bool cast_shadows;
	bool soft_shadows;
	float Bias;
};

struct PointLight {
	vec4 color; // color + intensity
	vec4 position; // pos + raduis
	bool cast_shadows;
	int  shadow_index;
	float Bias;
};

struct SpotLight {
	vec4 color; // color + intensity
	vec4 position; // pos + raduis
	vec4 direction; // dir + cutOff
	float outerCutOff;
	bool cast_shadows;
	int  shadow_index;
	float Bias;
};

struct FogEffect 
{
	bool use;
	vec3 color;
	float near;
	float far;
};


struct RefProbe 
{
	bool use_parallax_correction;
	vec3 mRefPos;
	vec3 mBoxMin;
	vec3 mBoxMax;
	samplerCube irradianceMap;
	samplerCube prefilterMap;
};

struct Material {
    vec3  albedo;
	float metallic;
	float roughness;
	float ao;

#ifndef DISPLACEMENT
	float emission_power;
#endif
	bool  use_emission;
	vec3  emission;
	bool use_tex_emission;
	sampler2D tex_emission;

	bool use_tex_metal;
	sampler2D tex_metal;

	bool use_tex_rough;
	sampler2D tex_rough;
	// textures
	bool use_tex_albedo;
	sampler2D tex_albedo;

	bool use_tex_normal;
	sampler2D tex_normal;
}; 

#ifdef DISPLACEMENT
uniform	float val1;
uniform	float val2;
#endif
/*
struct VisibleIndex {
	int index;
};

// Shader storage buffer objects
layout(std430, binding = 0) readonly buffer LightBuffer {
	PointLight data[];
} lightBuffer;


layout(std430, binding = 1) readonly buffer VisibleLightIndicesBuffer {
	VisibleIndex data[];
} visibleLightIndicesBuffer;


layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};
*/
// -------------- Vertex Data -------------- //
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
	vec2 TexCoords2;
	vec3 WorldPos;
	mat3 TBN;
	vec4 LightSpacePos[NUM_CASCADES];
	float ClipSpacePosZ;
	vec4 spot_MVP_SPACE[8];
	vec3 FragPosS;

#ifdef DISPLACEMENT
	float Blend;
#endif
} fs_in;


// -------------- Lightmap -------------- //
uniform bool use_lightmap;
uniform sampler2D tex_lightmap;

// -------------- Environmet Data -------------- //
uniform RefProbe env_probe;

// -------------- Engine Data -------------- //
uniform vec3 CamPos;
uniform bool use_alpha;

uniform bool selected;

uniform DirLight dirLight;
uniform PointLight[MAX_LIGHT_COUNT] p_lights;
uniform SpotLight[MAX_LIGHT_COUNT] sp_lights;

uniform sampler2DShadow shadowMaps[NUM_CASCADES];
uniform float CascadeEndClipSpace[NUM_CASCADES];

uniform samplerCube tex_shadows[8];
uniform sampler2DShadow texSpot_shadows[8];

uniform FogEffect Fog;

uniform int numberOfTilesX;

uniform int visible_pLights[MAX_LIGHT_COUNT];
uniform int visible_sLights[MAX_LIGHT_COUNT];

// -------------- Material -------------- //
uniform Material material;
//uniform Material material0;

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float SpotShadowCalculation(int idx, float bias, vec3 lightPos)
{	
	vec4 frag_pos_light = fs_in.spot_MVP_SPACE[idx];
	vec3 projCoords = frag_pos_light.xyz / frag_pos_light.w;
	projCoords = projCoords * 0.5 + 0.5;
	float shadow =  1.0 - textureProj( texSpot_shadows[idx], vec4(projCoords.xy, projCoords.z - bias, 1.0)) ;
	return shadow;
}

float PointShadowCalculation(vec3 fragPos, vec3 lightPos, float far_plane, float bias, int shadow_index)
{
    vec3 fragToLight = fragPos - lightPos;
    float closestDepth = texture(tex_shadows[shadow_index], fragToLight).r;
    closestDepth *= far_plane;
    float currentDepth = length(fragToLight);
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0; 
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

float GetShadowBias(vec3 N)
{
	float minBias = dirLight.Bias;
    float bias = max(minBias * (1.0 - dot(N, dirLight.direction)), minBias);
    return bias;
}

// https://www.gamedev.net/articles/programming/graphics/contact-hardening-soft-shadows-made-fast-r4906/
//
vec2 VogelDiskSample(int sampleIndex, int samplesCount, float phi) {
    float GoldenAngle = 2.4f;

    float r = sqrt(float(sampleIndex) + 0.5f) / sqrt(float(samplesCount));
    float theta = float(sampleIndex) * GoldenAngle + phi;

    float sine = sin(theta);
    float cosine = cos(theta);

    return vec2(r * cosine, r * sine);
}

// Optimal kernel scaling for a given shadow map size and sample count
//
// * shadowMapSize - Dimensions of the shadow texture
// * samplingKernelSize - Length of sampling kernel's side
//
vec2 VogelDiskScale(vec2 shadowMapSize, int samplingKernelSize) {
    vec2 texelSize = 2.0 / shadowMapSize;
    return texelSize * samplingKernelSize;
}

float InterleavedGradientNoise(vec2 screenCoordinate) {
    vec3 magic = vec3(0.06711056f, 0.00583715f, 52.9829189f);
    return fract(magic.z * fract(dot(screenCoordinate, magic.xy)));
}

float AvgBlockersDepthToPenumbra(float z_shadowMapView, float avgBlockersDepth, float lightArea) {
    return lightArea * (z_shadowMapView - avgBlockersDepth) / avgBlockersDepth;
}

float AvgBlockersDepthToPenumbra2(float z_shadowMapView, float avgBlockersDepth)
{
  float penumbra = (z_shadowMapView - avgBlockersDepth) / avgBlockersDepth;
  penumbra *= penumbra;
  return clamp(80.0f * penumbra,0.0,1.0);
  //return 80.0f * penumbra;
}

float Penumbra(float gradientNoise, vec2 shadowMapUV, float z_shadowMapView, int samplesCount, vec2 penumbraFilterMaxSize, int sh_indexX)
{
  float avgBlockersDepth = 0.0f;
  float blockersCount = 0.0f;

  for(int i = 0; i < samplesCount; i ++)
  {
    vec2 sampleUV = VogelDiskSample(i, samplesCount, gradientNoise);
    sampleUV = shadowMapUV + sampleUV * penumbraFilterMaxSize;

    //float sampleDepth = shadowMapTexture.SampleLevel(pointClampSampler, sampleUV, 0).x;
	float sampleDepth = texture(shadowMaps[sh_indexX], vec3(sampleUV, 0.0)).x;
    if(sampleDepth < z_shadowMapView)
    {
      avgBlockersDepth += sampleDepth;
      blockersCount += 1.0f;
    }
  }

  if(blockersCount > 0.0f)
  {
    avgBlockersDepth /= blockersCount;
    return AvgBlockersDepthToPenumbra2(z_shadowMapView, avgBlockersDepth);
  }
  else
  {
    return 0.0f;
  }
}

float ShadowCalculation(int sh_indexX, bool softS, vec3 N)
{
	int sh_index = sh_indexX;
	//float bias = dirLight.Bias;
	float bias = max(dirLight.Bias * (1.0 - dot(N, dirLight.direction)), 0.0001);
	
    vec3 projCoords = fs_in.LightSpacePos[sh_index].xyz;// / fs_in.LightSpacePos[sh_index].w;
    projCoords = projCoords * 0.5 + 0.5;
	float shadow = 0.0;

#if SHADOW_QUALITY == 1
	softS = false;
#endif

	if(softS)
	{
#if SHADOW_QUALITY == 2 
		// PCF
		vec2 texelSize = 1.5 / textureSize(shadowMaps[sh_index], 0);
	
		for(int x = -1; x <= 1; ++x)
		{
			for(int y = -1; y <= 1; ++y)
			{		
				shadow += (1.0 - texture( shadowMaps[sh_index], vec3(projCoords.xy + normalize(vec2(x, y)) * texelSize , projCoords.z - bias) ));
			}    
		}

		shadow /= 9.0;

#else
#if SHADOW_QUALITY == 3
	const int KernelSize = 4;
#elif SHADOW_QUALITY == 4
	const int KernelSize = 5;
#elif SHADOW_QUALITY == 5
	const int KernelSize = 6;
#endif

#if SHADOW_QUALITY != 1
		vec2 shadowMapUV = projCoords.xy;
    	const int VogelDiskSampleCount = KernelSize * KernelSize;

		float gradientNoise = InterleavedGradientNoise(gl_FragCoord.xy);
    	vec2 shadowMapSize = textureSize(shadowMaps[sh_index], 0).xy;
    	vec2 shadowFilterMaxSize = VogelDiskScale(shadowMapSize, KernelSize);
		//float smoothnessRadius = Penumbra(gradientNoise, shadowMapUV, z_shadowMapView, VogelDiskSampleCount, shadowFilterMaxSize, sh_indexX);//0.5;
    	float smoothnessRadius = 0.38;
		float shadow = 0.0f;

    	for(int i = 0; i < VogelDiskSampleCount; i++) {
        	vec2 sampleUV = VogelDiskSample(i, VogelDiskSampleCount, gradientNoise);
        	sampleUV = shadowMapUV + sampleUV * shadowFilterMaxSize * smoothnessRadius;
			shadow += (1.0 - texture(shadowMaps[sh_index], vec3(sampleUV, projCoords.z - bias)));
    	}
    
    	shadow /= float(VogelDiskSampleCount);
    	return shadow;
#endif

#endif

	}
	else
	{
		shadow += (1.0 - texture( shadowMaps[sh_index], vec3(projCoords.xy , projCoords.z - bias) ));
    }
	
    return shadow;
}

// -----------------------------------------------------------------------------------------------------------------------
const float PI = 3.14159265359;
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
float insideBox3d(vec3 vpos, vec3 bmin, vec3 bmax)
{
	vec3 s = step(bmin, vpos) - step(bmax, vpos);
	return s.x * s.y * s.z;
}

float inside_rectangle_smooth(vec3 p, vec3 bottom_left, vec3 top_right, float transition_area)
{
    vec3 s = smoothstep(bottom_left, bottom_left + vec3(transition_area), p) -
             smoothstep(top_right - vec3(transition_area), top_right, p);
    return(s.x * s.y * s.z);
}

float getBoxInfluenceWeight(vec3 localPos, vec3 probeCenter, float innerRange, float outerRange)
{
    // transform from World space to local box (without scaling, so we can test extend box)
    vec3 localPosition = localPos - probeCenter;
    // work in the upper left corner of the box.
    vec3 localDir = vec3(abs(localPosition.x), abs(localPosition.y), abs(localPosition.z));
    localDir = (localDir - innerRange) / (outerRange - innerRange);
    // Take max of all axis
    return max(localDir.x, max(localDir.y, localDir.z));
}
// -----------------------------------------------------------------------------------------------------------------------
vec3 CalcIBL(vec3 N, vec3 V, vec3 F0, vec3 albedo_color, float metallic_color, float roughness_color)
{
    //vec3 R = reflect(-V, N);
	
	vec3 ReflDirectionWS = reflect(-V, N);
	vec3 R = ReflDirectionWS;
	
	// Following is the parallax-correction code
	if(env_probe.use_parallax_correction)
	{
		// Find the ray intersection with box plane
		vec3 FirstPlaneIntersect = (env_probe.mBoxMax - fs_in.FragPos) / ReflDirectionWS;
		vec3 SecondPlaneIntersect = (env_probe.mBoxMin - fs_in.FragPos) / ReflDirectionWS;
		// Get the furthest of these intersections along the ray
		vec3 FurthestPlane = max(FirstPlaneIntersect, SecondPlaneIntersect);
		// Find the closest far intersection
		float distnce = min(min(FurthestPlane.x, FurthestPlane.y), FurthestPlane.z);
		// Get the intersection position
		vec3 IntersectPositionWS = fs_in.FragPos + ReflDirectionWS * distnce;
		// Get corrected reflection
		ReflDirectionWS = IntersectPositionWS - env_probe.mRefPos;
		// End parallax-correction code
		
		//if(insideBox3d(fs_in.FragPos, env_probe.mBoxMin, env_probe.mBoxMax) == 1) 
		//R = ReflDirectionWS;
		R = mix(R, ReflDirectionWS, insideBox3d(fs_in.FragPos, env_probe.mBoxMin, env_probe.mBoxMax));
		//R = mix(R, ReflDirectionWS, inside_rectangle_smooth(fs_in.FragPos, env_probe.mBoxMin, env_probe.mBoxMax, 1.0));
		//R = mix(R, ReflDirectionWS, getBoxInfluenceWeight(fs_in.FragPos, env_probe.mRefPos, 1.0, 1.0));
	}
	
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness_color);

    vec3 kD = (1.0 - F) * (1.0 - metallic_color);
    
    vec3 irradiance = texture(env_probe.irradianceMap, N).rgb;
    vec3 diffuse    = irradiance * albedo_color;
    
    vec3 prefilteredColor = textureLod(env_probe.prefilterMap, R,  roughness_color * 4).rgb;      
    vec3 specular = prefilteredColor * F;

	float _AO = material.ao;
	if(use_lightmap)
		_AO *= pow(texture(tex_lightmap, fs_in.TexCoords2).r, _AO);
	
	//if(roughness_color == 1)	specular = vec3(0,0,0);
	diffuse *= _AO;
	return  (kD * diffuse + specular);// * _AO;
}
// -----------------------------------------------------------------------------------------------------------------------
// Settings
/*const uint  g_sss_max_steps        = 16;     // Max ray steps, affects quality and performance.
const float g_sss_ray_max_distnce = 0.05f;  // Max shadow length, longer shadows are less accurate.
const float g_sss_thickness        = 0.02f;  // Depth testing thickness.
const float g_sss_step_length      = g_sss_ray_max_distnce / (float)g_sss_max_steps;

uniform mat4 view;
uniform mat4 projection;

float ScreenSpaceShadows(vec3 fpos, vec3 lightdir)
{
    // Compute ray position and direction (in view-space)
    vec3 ray_pos = (vec4(fpos, 1.0)* view).xyz;
    vec3 ray_dir = (vec4(-lightdir, 0.0)* view).xyz;

    // Compute ray step
    vec3 ray_step = ray_dir * g_sss_step_length;
	
    // Ray march towards the light
    float occlusion = 0.0;
    vec2 ray_uv   = vec2(0.0, 0.0);
    for (uint i = 0; i < g_sss_max_steps; i++)
    {
        // Step the ray
        ray_pos += ray_step;
        ray_uv  = project_uv(ray_pos, projection);

        // Ensure the UV coordinates are inside the screen
       // if (is_saturated(ray_uv))
        //{
            // Compute the difference between the ray's and the camera's depth
            float depth_z     = get_linear_depth(ray_uv);
            float depth_delta = ray_pos.z - depth_z;

            // Check if the camera can't "see" the ray (ray depth must be larger than the camera depth, so positive depth_delta)
            if ((depth_delta > 0.0f) && (depth_delta < g_sss_thickness))
            {
                // Mark as occluded
                occlusion = 1.0f;

                // Fade out as we approach the edges of the screen
                //occlusion *= screen_fade(ray_uv);

                break;
            }
      //  }
    }

    // Convert to visibility
    return 1.0f - occlusion;
}*/
vec3 CalcDirLight(DirLight light, vec3 V, vec3 N, vec3 F0, vec3 albedo_color, float metallic_color, float roughness_color)
{
	float shadow = 0;
	
	if(light.cast_shadows)
	{
		for (int i = 0 ; i < NUM_CASCADES ; i++) 
		{
			if (fs_in.ClipSpacePosZ <= CascadeEndClipSpace[i])
			{
				shadow = ShadowCalculation(i, light.soft_shadows, N);
				break;
			}
		}
    }
	
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
	
	return ((1.0 - shadow) * (kD * albedo_color / PI + specular) * vec3(1.0) * NdotL) * light.intensity * 3 * light.color;
}
// -----------------------------------------------------------------------------------------------------------------------
vec3 CalcPointLight(PointLight light, vec3 V, vec3 N, vec3 F0, vec3 albedo_color, float metallic_color, float roughness_color)
{
	float shadow = (light.cast_shadows && light.shadow_index >= 0) ? PointShadowCalculation(fs_in.FragPos, light.position.xyz, light.position.w, light.Bias, light.shadow_index) : 0.0;                      
	vec3 lpos = light.position.xyz;
	vec3 lcol =  light.color.rgb;
	float lrad = light.position.w;
    float distnce = length(lpos - fs_in.FragPos);

	if(distnce > lrad) return vec3(0);

	vec3 L = normalize(lpos - fs_in.FragPos);
    vec3 H = normalize(V + L);

	float compression = 1.5;
	float attenuation = pow(smoothstep(lrad, 0, distnce), compression);

  	vec3 radiance = lcol * attenuation * light.color.w;    

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
	
	return ((1.0 - shadow) * ( kD * albedo_color / PI + specular)) * radiance * NdotL;
}
// -----------------------------------------------------------------------------------------------------------------------
vec3 CalcSpotLight(SpotLight light, vec3 V, vec3 N, vec3 F0, vec3 albedo_color, float metallic_color, float roughness_color)
{
	vec3 lpos = light.position.xyz;
	float lrad = light.position.w;
	
	float shadow = (light.cast_shadows && light.shadow_index >= 0) ? SpotShadowCalculation(light.shadow_index, light.Bias, lpos) : 0.0;     
	//float shadow = SpotShadowCalculation(light.shadow_index, light.Bias, lpos);  
    float distnce = length(lpos - fs_in.FragPos);
	if(distnce > lrad) return vec3(0);
	
	vec3 L = normalize(lpos - fs_in.FragPos);
	float theta = dot(L, normalize(-light.direction.xyz));
	
	//if(theta < light.outerCutOff) return vec3(0); // we're working with angles as cosines instead of degrees so a '>' is used.
	
	// spotlight (soft edges)
    float epsilon = (light.direction.w - light.outerCutOff);
    float intensitys = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	vec3 H = normalize(V + L);
	
	float compression = 1.5;
	float attenuation = pow(smoothstep(lrad, 0, distnce), compression);
	
    vec3 radiance = light.color.rgb * attenuation * light.color.w * intensitys;
	
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
	
	return (1.0 - shadow) * ( kD * albedo_color / PI + specular) * radiance * NdotL;
}
// -----------------------------------------------------------------------------------------------------------------------
vec3 CalcFog()
{	
	vec3 fg = vec3(0.0);
	float distnce = length(CamPos - fs_in.FragPos);
	if(distnce < Fog.near) return vec3(0);
	fg = Fog.color;

   // fg = mix(Fog.color, vec3(1,1,1) * 2, 0.7 * pow(max(dot(normalize(fs_in.WorldPos), sun_direction), 0.0), 8.0));

	//fg = mix(Fog.color.rgb, dirLight.color.rgb, dirLight.intensity * pow(max(dot(CamPos, dirLight.direction), 0.0), 8.0));

	float attenuation = ((distnce - Fog.near) / (Fog.far - Fog.near));
	attenuation = clamp(attenuation, 0.0, 1.0); 
	
	fg *= attenuation;

	return fg;
}
// -----------------------------------------------------------------------------------------------------------------------
vec3 getNormalFromMap()
{
#ifndef DISPLACEMENT
    vec3 tangentNormal = texture(material.tex_normal, fs_in.TexCoords).xyz * 2.0 - 1.0;
	return normalize(fs_in.TBN * tangentNormal);
#else
	vec3 N1 ;
	vec3 N2 ;

	if(material.use_tex_normal) N1 = (fs_in.TBN * (texture(material.tex_normal, fs_in.TexCoords).xyz * 2.0 - 1.0));
	else N1 = (fs_in.Normal);

	if(material.use_emission) N2 = (fs_in.TBN * (texture(material.tex_emission, fs_in.TexCoords).xyz * 2.0 - 1.0));
	else N2 = (fs_in.Normal);

	return normalize(mix(N1, N2, fs_in.Blend));
#endif
}
// -----------------------------------------------------------------------------------------------------------------------
/*void mains()
{
	vec3 V = normalize(CamPos - fs_in.FragPos);	    // View Direction
	vec3 N = normalize(fs_in.Normal);
	vec3 ReflDirectionWS = reflect(-V, N);
	vec3 R = ReflDirectionWS;
    vec3 prefilteredColor = textureLod(env_probe.prefilterMap, R,  material.roughness * 4).rgb;      

	if (material.use_emission)
	{
		// emission
		vec3 emi = material.emission * material.emission_power;
		prefilteredColor += emi;
	}

	FragColor = vec4(prefilteredColor , 1.0);
}
void mainss()
{  
	float d = gl_FragCoord.z / gl_FragCoord.w; 
	float depthValue = texture(tex_lightmap, (gl_FragCoord.xy / resolution.xy)).r / gl_FragCoord.w;
	d = depthValue;
	FragColor = vec4(d, d, d, 1.0);
}*/
void main()
{  
	gPosition = fs_in.FragPosS;

 #ifndef DISPLACEMENT
	if(use_alpha)
	{
		vec4 tr = texture(material.tex_emission, fs_in.TexCoords);
	 	if((tr.r) < 0.5)
       		discard;
	}
#endif
	//FragColor = vec4(1.0, 1.0, 1.0 , 1.0); return;
    // Get material data
	//--------------------------------------------
    vec3  albedo_color    = material.albedo;
    float metallic_color  = material.metallic;
    float roughness_color = material.roughness;
	vec3 N; 										// Normal
	vec3 V = normalize(CamPos - fs_in.FragPos);	    // View Direction
	
#ifndef DISPLACEMENT
	if(material.use_tex_albedo)
	albedo_color *= pow(texture(material.tex_albedo, fs_in.TexCoords).rgb, vec3(2.2));
	
	if(material.use_tex_metal)
    metallic_color *= texture(material.tex_metal, fs_in.TexCoords).r;
	
	if(material.use_tex_rough)
    roughness_color *= texture(material.tex_rough, fs_in.TexCoords).r;
	
	if(material.use_tex_normal) N = getNormalFromMap();
	else 
		N = normalize(fs_in.Normal);
#else
	metallic_color = mix(material.metallic, val2, fs_in.Blend);
	albedo_color = mix(albedo_color, material.emission, fs_in.Blend);

	vec3 albedo_color1 = material.albedo;
	vec3 albedo_color2 = material.emission;

	if(material.use_tex_albedo)	
		albedo_color1 *=  pow(texture(material.tex_albedo, fs_in.TexCoords).rgb, vec3(2.2));
	if(material.use_tex_metal)
		albedo_color2 *=  pow(texture(material.tex_metal, fs_in.TexCoords).rgb, vec3(2.2));

	albedo_color *= mix(albedo_color1, albedo_color2, fs_in.Blend);

	if(material.use_tex_rough)
    	roughness_color *= texture(material.tex_rough, fs_in.TexCoords).r;

	roughness_color = mix(roughness_color, val1, fs_in.Blend);

	N = getNormalFromMap();
#endif

	//if(texture(material.tex_emission, fs_in.TexCoords).r == 1)
	//	discard;
		
	// Ambient Lighting (IBL)
	//--------------------------------------------
	vec3 F0 = mix(vec3(0.04), albedo_color, metallic_color); 
    vec3 ambient = CalcIBL(N, V, F0, albedo_color, metallic_color, roughness_color);
	
	// Direct Lighting
	//--------------------------------------------
    vec3 Lo = vec3(0.0);
	
	if(dirLight.use) 
	Lo += CalcDirLight(dirLight, V, N, F0, albedo_color, metallic_color, roughness_color);
	
	for(int i = 0; i < MAX_LIGHT_COUNT && visible_pLights[i] != -1; i++)
	{
		Lo += CalcPointLight(p_lights[visible_pLights[i]], V, N, F0, albedo_color, metallic_color, roughness_color);
	}
	
	for(int i = 0; i < MAX_LIGHT_COUNT && visible_sLights[i] != -1; i++)
	{
		Lo += CalcSpotLight(sp_lights[i], V, N, F0, albedo_color, metallic_color, roughness_color);
	}
	
	// Forward+ lighting (testing)
	//--------------------------------------------
	// Determine which tile this pixel belongs to
	/*ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(16, 16);
	uint index = tileID.y * numberOfTilesX + tileID.x;*/

	//vec3 viewDirection = normalize(fragment_in.tangentViewPosition - fragment_in.tangentFragmentPosition);

	// The offset is this tile's position in the global array of valid light indices.
	// Loop through all these indices until we hit max number of lights or the end (indicated by an index of -1)
	// Calculate the lighting contribution from each visible point light
	/*uint offset = index * 1024;
	for (int i = 0; i < 1024 && visibleLightIndicesBuffer.data[offset + i].index != -1; i++) 
	{
		int lightIndex = visibleLightIndicesBuffer.data[offset + i].index;
		PointLight light = lightBuffer.data[lightIndex];
		
		Lo += CalcPointLight(light, V, N, F0, albedo_color, metallic_color, roughness_color);
	}*/
	
	// Final Shading
	//--------------------------------------------
	vec3 color = ambient + Lo;
	
#ifndef DISPLACEMENT
	if (material.use_emission)
	{
		// emission
		vec3 emi = material.emission * material.emission_power;
		if(material.use_tex_emission){
			emi *= texture(material.tex_emission, fs_in.TexCoords).r;
		}
		color += emi;
	}
#endif
	// apply fog
	if(Fog.use) color += CalcFog();
	
	// HDR tonemapping
   // color = color / (color + vec3(1.0));
    // gamma correct
   // color = pow(color, vec3(1.0/2.2));
	
	/*vec4 CascadeIndicator = vec4(0.0, 0.0, 0.0, 0.0);

	for (int i = 0 ; i < NUM_CASCADES ; i++) 
	{
		if (fs_in.ClipSpacePosZ <= CascadeEndClipSpace[i])
		{
			 if (i == 0) 
                CascadeIndicator = vec4(0.1, 0.0, 0.0, 1.0);
            else if (i == 1)
                CascadeIndicator = vec4(0.0, 0.1, 0.0, 1.0);
            else if (i == 2)
                CascadeIndicator = vec4(0.0, 0.0, 0.1, 1.0);
			break;
		}
	}*/

	// Rim effect
   // float intensity = 0.2 - max(dot(V, N), 0.0);
   // intensity = max(0.0, intensity); // ignore rim light if negative
   // vec4 RimColor = vec4(intensity * vec3(0.9, 0.8, 0.0) * 20, 1.0);
    FragColor = vec4(color , 1.0);
	//FragColor += RimColor;
}

#endif // COMPILING_FS