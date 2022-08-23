
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

layout (location = 6) in float aBlend;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
	vec3 WorldPos;
	mat3 TBN;
	float Blend;
} vs_out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
	mat4 VP; // projection * view
};

uniform mat4 models[1000];

void main()
{
	vs_out.Blend = aBlend;

	mat4 model = models[gl_InstanceID];
	vs_out.TexCoords = aTexCoords * tex_uv;
	
	vs_out.WorldPos = aPos;
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));

	vec4 viewPos = view * model * vec4(aPos, 1.0);
	
	vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
	vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
	vs_out.TBN = mat3(T, B, N);

    gl_Position = VP * model * vec4(aPos, 1.0);
}

#endif // COMPILING_VS

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS
//#extension GL_ARB_shader_storage_buffer_object : enable
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 gPosition;

// -------------- Vertex Data -------------- //
in VS_OUT {
   vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
	vec3 WorldPos;
	mat3 TBN;
	float Blend;
} fs_in;


void main()
{  
	gPosition = fs_in.FragPosS;

 #ifndef DISPLACEMENT
/*	if(use_alpha)
	{
		vec4 tr = texture(material.tex_emission, fs_in.TexCoords);
	 	if((tr.r) < 0.5)
       		discard;
	}*/
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
	{
		vec4 col = texture(material.tex_albedo, fs_in.TexCoords);
		albedo_color *= pow(col.rgb, vec3(2.2));

		if(use_alpha)
		{
	 		if((col.a) < 0.5)
       			discard;
		}
	}
	
	if(material.use_tex_metal)
    metallic_color *= texture(material.tex_metal, fs_in.TexCoords).r;
	
	if(material.use_tex_rough)
    roughness_color *= texture(material.tex_rough, fs_in.TexCoords).r;
	
	if(material.use_tex_normal) N = getNormalFromMap();
	else 
		N = normalize(fs_in.Normal);
#else
	if(material.use_tex_albedo)
		albedo_color *= pow(texture(material.tex_albedo, fs_in.TexCoords).rgb, vec3(2.2));
	
  //	metallic_color  = mix(0.0, 1.0, fs_in.Blend);
    //roughness_color = mix(material.roughness, 0.0, fs_in.Blend);
/*	metallic_color = mix(material.metallic, val2, fs_in.Blend);
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
*/
	if(material.use_tex_normal) 
		N = getNormalFromMap();
	else 
		N = normalize(fs_in.Normal);
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
		Lo += CalcSpotLight(sp_lights[visible_sLights[i]], V, N, F0, albedo_color, metallic_color, roughness_color);
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