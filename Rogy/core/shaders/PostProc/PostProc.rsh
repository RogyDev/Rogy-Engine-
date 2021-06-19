// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
}  

#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS

#define SAMPLE_COUNT 9
uniform vec2 Pixels[SAMPLE_COUNT];

out vec4 FragColor;

in vec2 TexCoords;

float middleGrey = 0.18f;

uniform sampler2D screenTexture;
uniform vec2 u_resolution;

uniform float p_exposure;

// Vignette
uniform bool v_use;
uniform float v_radius;
uniform float v_softness;

// Color Correction
uniform bool filmic;
uniform bool cc_use;
uniform float brightness;
uniform float contrast;
uniform float saturation;

// SSAO
uniform sampler2D ssao;
uniform bool ssao_use;

// Motion Blur
uniform bool UseMB;
uniform vec2 MBvelocity;
uniform float motionBlurScale;

vec3 applyVignette(vec3 color)
{
   /* vec2 position = (gl_FragCoord.xy / u_resolution) - vec2(0.5);           
    float dist = length(position);
    
    float vignette = smoothstep(v_radius, v_radius - v_softness, dist);

    color.rgb = color.rgb - (1.0 - vignette);
*/
	vec2 distance_xy = TexCoords - vec2(0.5f);
	distance_xy *= vec2(1600/900, v_radius);
	distance_xy /= 2.0f;
	float distance = dot(distance_xy, distance_xy);
	color.rgb *= (1.0 + pow(distance, v_softness) * -2.0); //pow - multiply
	
    /*vec2 tuv = TexCoords * (vec2(1.0) - TexCoords.yx);
    float vign = tuv.x*tuv.y * v_radius;
    vign = pow(vign, v_softness);
    color *= vign;*/
	
    return color;
}

vec3 computeMotionBlur(vec3 colorVector)
{
    vec2 texelSize = 1.0f / vec2(textureSize(screenTexture, 0));
	
	int motionBlurMaxSamples = 60;
	
    vec2 velocity = MBvelocity;
    velocity *= motionBlurScale;

    float fragSpeed = length(velocity / texelSize);
    int numSamples = clamp(int(fragSpeed), 1, motionBlurMaxSamples);

    for (int i = 1; i < numSamples; ++i)
    {
        vec2 blurOffset = velocity * (float(i) / float(numSamples - 1) - 0.5f);
        colorVector += texture(screenTexture, TexCoords + blurOffset).rgb;
    }

    return colorVector /= float(numSamples);
}

float computeSOBExposure(float aperture, float shutterSpeed, float iso)
{
    float lAvg = (1000.0f / 65.0f) * sqrt(aperture) / (iso * shutterSpeed);

    return middleGrey / lAvg;
}

vec3 ReinhardTM(vec3 color)
{
    return color / (color + vec3(1.0f));
}

vec3 UnchartedTM(vec3 color)
{
  const float A = 0.15f;
  const float B = 0.50f;
  const float C = 0.10f;
  const float D = 0.20f;
  const float E = 0.02f;
  const float F = 0.30f;
  const float W = 11.2f;

  color = ((color * (A * color + C * B) + D * E) / (color * ( A * color + B) + D * F)) - E / F;

  return color;
}


vec3 FilmicTM(vec3 color)
{
    color = max(vec3(0.0f), color - vec3(0.004f));
    color = (color * (p_exposure * color + 0.5f)) / (color * (p_exposure * color + 1.7f) + 0.06f);

    return color;
}

vec3 HDRTM(vec3 color)
{
	// exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-color * p_exposure);
	 // gamma correction 
    mapped = pow(mapped, vec3(1.0 / 2.2));
    return mapped;
}

void main()
{     
	vec3 color = texture(screenTexture, TexCoords).rgb;
	
	//if(UseMB)
		//color = computeMotionBlur(color);
	
	color = FilmicTM(color);
	//color = HDRTM(color);
	
	if(cc_use)
	{
		// contrast
		vec3 colorContrasted = color.rgb * contrast;
		// brightness
		color = colorContrasted + vec3(brightness, brightness, brightness);
		// saturation
		float luminance = color.r*0.299 + color.g*0.587 + color.b*0.114;
		color = mix(color, vec3(luminance), -saturation);
	}
	
	if(v_use)
	{
		color = applyVignette(color);
	}
	
	if(ssao_use)
	{
		float AmbientOcclusion = texture(ssao, TexCoords).r;
		color *= AmbientOcclusion;
	}
   
	// Exposure computation
    //color *= computeSOBExposure(16.0, 0.5, 1000.0);
	
	//float exposure = 2.50;
	// exposure tone mapping
    //vec3 mapped = (color * exposure);
    // gamma correction 
    //mapped = pow(mapped, vec3(1.0 / 2.2));
	
	//color = mapped;
	/*vec3  avg     = vec3(0);
	
	for( unsigned int i = 0; i < SAMPLE_COUNT; i++ )
	{
		avg += texture(screenTexture, Pixels[i]).rgb;
	}
	
	avg /= SAMPLE_COUNT;
	float Avg_Bright = max( max(avg.x, avg.y), avg.z );
	float exposure = Avg_Bright * 2.2;
	//vec3 coloreEX = color.rgb * Avg_Bright;
	
	// += coloreEX;
	
	*/
	
	//color /= p_exposure;
	
	FragColor = vec4(color, 1.0);
} 

#endif