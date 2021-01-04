
// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 UV;
out vec4 LightSpacePos;

uniform vec3 CameraRight_worldspace;
uniform mat4 VP; 
uniform vec3 BillboardPos; 
uniform vec2 BillboardSize;

// Shadowing
uniform mat4 gLightWVP;

void main()
{
	vec3 particleCenter_wordspace = BillboardPos;
	
	vec3 vertexPosition_worldspace = particleCenter_wordspace 
	+ CameraRight_worldspace * aPos.x * BillboardSize.x 
	+ vec3(0.0, 1.0, 0.0) * aPos.y * BillboardSize.y;


	gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);

	UV = aTexCoords;//aPos + vec2(0.5, 0.5);
	
	vec4 Pos = vec4(vertexPosition_worldspace, 1.0);
	LightSpacePos = gLightWVP * Pos;
}

#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS
in vec2 UV;
out vec4 color;

in vec4 LightSpacePos;
uniform sampler2DShadow shadowMaps;

uniform sampler2D BBTexture;
uniform vec3 Tex_color;
uniform float alpaValue;
uniform bool use_shadows;

float ShadowCalculation()
{
	vec4 fragPosLightSpace = LightSpacePos;
	
	float bias = 0.005;

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    projCoords = projCoords * 0.5 + 0.5;
	
	float shadow = 0.0;
	
	shadow += (1.0 - texture( shadowMaps, vec3(projCoords.xy , projCoords.z - bias) ));
	
	shadow = clamp(shadow, 0.0, 0.5);
	
    return shadow;
}

void main()
{
	vec4 tex = texture( BBTexture, UV );
    if(tex.a < alpaValue)
        discard;
	
	float shadow = 1.0f;
	if(use_shadows)
	{
		shadow = ShadowCalculation();
	}
	vec3 col = tex.rgb * (1.0 - shadow);
	color = vec4(col, 1.0f);
	
    //color = tex;
}
#endif