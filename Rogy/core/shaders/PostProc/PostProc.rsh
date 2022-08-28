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

uniform sampler2D HighlightTex;

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

// FXAA
uniform bool use_fxaa;

// Sharping
uniform bool sharpen;
uniform float sharpen_amount;


// SSAO
uniform sampler2D ssao;
uniform bool ssao_use;

// Bloom
uniform sampler2D bloomBlur;
uniform sampler2D bloomBlur2;
uniform bool bloom_use;

// Motion Blur
uniform bool UseMB;
uniform vec2 MBvelocity;
uniform float motionBlurScale;

uniform int ToneMap;
uniform float CamFar;

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

vec3 ReinhardTM(vec3 x)
{
   // return x / (p_exposure + x);
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    x = x * p_exposure;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
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

vec3 UnrealTM(vec3 x) {
  vec3 X = max(vec3(0.0), x - 0.004);
  vec3 result = (X * (p_exposure* X + 0.5)) / (X * (p_exposure * X + 1.7) + 0.06);
  return pow(result, vec3(2.2));
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

// ----------------------------------------------------------//
vec3 reinhard(vec3 hdrColor){
	return hdrColor / (1.0 + hdrColor);
}

vec3 simpleExposure(vec3 hdrColor, float exposure){
	return 1.0 - exp(-hdrColor * exposure);
}

vec3 aces(vec3 hdrColor){
	return clamp(((0.9036 * hdrColor + 0.018) * hdrColor) / ((0.8748 * hdrColor + 0.354) * hdrColor + 0.14), 0.0, 1.0);
}

vec3 cineon(vec3 hdrColor){
	vec3 shiftedColor = max(vec3(0.0), hdrColor - 0.004);
	return (shiftedColor * (6.2 * shiftedColor + 0.5)) / (shiftedColor * (6.2 * shiftedColor + 1.7) + 0.06);
}

vec3 uncharted2(vec3 hdrColor){
	vec3 x = 2.0 * hdrColor; // Exposure bias.
	vec3 newColor = ((x * (0.15 * x + 0.05) + 0.004) / (x * (0.15 * x + 0.5) + 0.06)) - 0.02/0.3;
	return newColor * 1.3790642467; // White scale
}

vec4 QThreshold(vec4 color, vec3 curve, float exposure, float threshold)
{
    float br = max(max(color.r, color.g), color.b);
    float rq = clamp(br - curve.x, 0.0, curve.y);
    rq = curve.z * rq * rq;
    color.rgb *= max(rq, br - threshold) / max(br, 0.0001);
    return color * exposure;
}

uniform float Far;
uniform float Near;

// Linearize the depth because we are using the projection
float LinearizeDepth(float depth) {
	float z = depth * 2.0 - 1.0;
	return (2.0 * Near * Far) / (Far + Near - z * (Far - Near));
}

vec4 SSharpen(in sampler2D tex, in vec2 coords, in vec2 renderSize) {
  float dx = (1.0 / renderSize.x) * sharpen_amount;
  float dy = (1.0 / renderSize.y) * sharpen_amount;
  vec4 sum = vec4(0.0);
  sum += -1. * texture2D(tex, coords + vec2( -1.0 * dx , 0.0 * dy));
  sum += -1. * texture2D(tex, coords + vec2( 0.0 * dx , -1.0 * dy));
  sum += 5. * texture2D(tex, coords + vec2( 0.0 * dx , 0.0 * dy));
  sum += -1. * texture2D(tex, coords + vec2( 0.0 * dx , 1.0 * dy));
  sum += -1. * texture2D(tex, coords + vec2( 1.0 * dx , 0.0 * dy));
  
  return sum;
}



vec3 computeFxaa()
{
    vec2 frameBufSize = u_resolution;
    float FXAA_SPAN_MAX = 8.0;
    float FXAA_REDUCE_MUL = 1.0/8.0;
    float FXAA_REDUCE_MIN = 1.0/128.0;

    vec3 rgbNW=texture2D(screenTexture,TexCoords+(vec2(-1.0,-1.0)/frameBufSize)).xyz;
    vec3 rgbNE=texture2D(screenTexture,TexCoords+(vec2(1.0,-1.0)/frameBufSize)).xyz;
    vec3 rgbSW=texture2D(screenTexture,TexCoords+(vec2(-1.0,1.0)/frameBufSize)).xyz;
    vec3 rgbSE=texture2D(screenTexture,TexCoords+(vec2(1.0,1.0)/frameBufSize)).xyz;
    vec3 rgbM=texture2D(screenTexture,TexCoords).xyz;

    vec3 luma=vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max(
        (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),
        FXAA_REDUCE_MIN);

    float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = min(vec2( FXAA_SPAN_MAX,  FXAA_SPAN_MAX),
          max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
          dir * rcpDirMin)) / frameBufSize;

    vec3 rgbA = (1.0/2.0) * (
        texture2D(screenTexture, TexCoords.xy + dir * (1.0/3.0 - 0.5)).xyz +
        texture2D(screenTexture, TexCoords.xy + dir * (2.0/3.0 - 0.5)).xyz);
    vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
        texture2D(screenTexture, TexCoords.xy + dir * (0.0/3.0 - 0.5)).xyz +
        texture2D(screenTexture, TexCoords.xy + dir * (3.0/3.0 - 0.5)).xyz);
    float lumaB = dot(rgbB, luma);

    if((lumaB < lumaMin) || (lumaB > lumaMax)){
       return rgbA;
    }else{
        return rgbB;
    }
}

// From http://filmicgames.com/archives/75
vec3 Uncharted2Tonemap(vec3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((x*p_exposure*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec3 ReinhardToneMap(vec3 x)
{
    return (vec3(1.0) - exp(-x * p_exposure));
}

//http://filmicworlds.com/blog/filmic-tonemapping-operators/
vec3 FilmicToneMap(vec3 x) 
{
  vec3 X = max(vec3(0.0), (x * p_exposure) - 0.004);
  vec3 result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
  return pow(result, vec3(2.2));
}

void mains()
{   
    vec3 color = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(color, 1.0);
}
void mainss()
{   
    float depth = texture(screenTexture, TexCoords).r;
    float v = LinearizeDepth(depth) / Far;
    FragColor = vec4(v, v, v, 1.0);
}
   
void main()
{     
	/*float depth = texture(screenTexture, TexCoords).r;
	float v = LinearizeDepth(depth) / Far;
	FragColor = vec4(v, v, v, 1.0); return;*/

	vec3 color;
	
	
	//if(use_fxaa)
		// color = computeFxaa();
	//else
	//{
		if(sharpen)
			color = SSharpen(screenTexture, TexCoords, u_resolution).rgb;
		else
        {
			color = texture(screenTexture, TexCoords).rgb;
            
            // Cartoon Edge detection
            /*vec3 normCol = texture(screenTexture, TexCoords).rgb;
            vec3 offsetCol = texture(screenTexture, TexCoords - vec2(0.002, 0.002)).rgb;
            if(length(normCol - offsetCol) > 0.1)
            {
                normCol = vec3(1.0,0.95,0.001);
            }
            color = normCol;*/

          
        }
//	}

    // "HighlightTex" contains only the objects we wnat to outline
   /* float normCol = texture(HighlightTex, TexCoords).r;
    float offsetCol1 = texture(HighlightTex, TexCoords - vec2(0.000, 0.002)).r;
    float offsetCol2 = texture(HighlightTex, TexCoords - vec2(0.002, 0.000)).r;
    float offsetCol3 = texture(HighlightTex, TexCoords - vec2(0.000, -0.002)).r;
    float offsetCol4= texture(HighlightTex, TexCoords - vec2(-0.002, 0.000)).r;
    if((normCol - offsetCol1) > 0.1 || (normCol - offsetCol2) > 0.1 || (normCol - offsetCol3) > 0.1 || (normCol - offsetCol4) > 0.1)
    {
        color = vec3(0.8,0.7,0.001);
    }
*/
	if(ssao_use)
	{
		float AmbientOcclusion = texture(ssao, TexCoords).r;
		color *= AmbientOcclusion;
	}

	if(bloom_use)
	{
		vec3 bloomColor = texture(bloomBlur, TexCoords).rgb * 0.75;
        vec3 bloomColor2 = texture(bloomBlur2, TexCoords).rgb * 0.5;
        //color += bloomColor + bloomColor2; // additive blending
        color = mix(color, bloomColor + bloomColor2, 0.1);
	}	
	
	if(ToneMap == 0)
		color = FilmicToneMap(color);		
	else if	(ToneMap == 1)
		color = ReinhardTM(color);		
	else if(ToneMap == 2)
		color = cineon(color);
	else if(ToneMap == 3)
		color = aces(color);

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
	
    //************************************
    // Gamma Correction 
    //************************************
	const float gamma = 2.2;
    color = pow(color, vec3(1.0 / gamma));

	FragColor = vec4(color, 1.0);
} 

#endif