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
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform vec2 u_resolution;
uniform float sharpen_amount;

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
    float FXAA_SPAN_MAX = 6.0;
    float FXAA_REDUCE_MUL = 1.0/6.0;
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


void main()
{     
	vec3 color = computeFxaa();//SSharpen(screenTexture, TexCoords, u_resolution).rgb;
	
	FragColor = vec4(color, 1.0);
} 


/*
// Settings for FXAA.
#define EDGE_THRESHOLD_MIN 0.0312
#define EDGE_THRESHOLD_MAX 0.125
#define QUALITY(q) ((q) < 5 ? 1.0 : ((q) > 5 ? ((q) < 10 ? 2.0 : ((q) < 11 ? 4.0 : 8.0)) : 1.5))
#define ITERATIONS 12
#define SUBPIXEL_QUALITY 0.75


#extension GL_ARB_bindless_texture : enable

float rgb2luma(vec3 rgb){
	return sqrt(dot(rgb, vec3(0.299, 0.587, 0.114)));
}

vec3 computeFxaa3()
{
   vec2 uv = TexCoords;
   sampler sClampLinear;
   vec2 inverseScreenSize = vec2(1.0 / u_resolution.x, 1.0 / u_resolution.y);
   vec3 colorCenter = texture(sampler2D(screenTexture, sClampLinear), uv).rgb;
	
	// Luma at the current fragment
	float lumaCenter = rgb2luma(colorCenter);
	
	// Luma at the four direct neighbours of the current fragment.
    //vec3 rgbNW=texture2D(screenTexture,TexCoords+(vec2(-1.0,-1.0)/frameBufSize)).xyz;
    //float lumaDown 	= rgb2luma(textureLodOffset(texture2D(screenTexture, sClampLinear), uv, 0.0,ivec2( 0,-1)).rgb);
	float lumaDown 	= rgb2luma(textureLodOffset(sampler2D(screenTexture, sClampLinear), uv, 0.0,ivec2( 0,-1)).rgb);
	float lumaUp 	= rgb2luma(textureLodOffset(sampler2D(screenTexture, sClampLinear), uv, 0.0,ivec2( 0, 1)).rgb);
	float lumaLeft 	= rgb2luma(textureLodOffset(sampler2D(screenTexture, sClampLinear), uv, 0.0,ivec2(-1, 0)).rgb);
	float lumaRight = rgb2luma(textureLodOffset(sampler2D(screenTexture, sClampLinear), uv, 0.0,ivec2( 1, 0)).rgb);
	
	// Find the maximum and minimum luma around the current fragment.
	float lumaMin = min(lumaCenter, min(min(lumaDown, lumaUp), min(lumaLeft, lumaRight)));
	float lumaMax = max(lumaCenter, max(max(lumaDown, lumaUp), max(lumaLeft, lumaRight)));
	
	// Compute the delta.
	float lumaRange = lumaMax - lumaMin;
	
	// If the luma variation is lower that a threshold (or if we are in a really dark area), we are not on an edge, don't perform any AA.
	if(lumaRange < max(EDGE_THRESHOLD_MIN, lumaMax * EDGE_THRESHOLD_MAX)){
		return colorCenter;
	}
	
	// Query the 4 remaining corners lumas.
	float lumaDownLeft 	= rgb2luma(textureLodOffset(sampler2D(screenTexture, sClampLinear), uv, 0.0,ivec2(-1,-1)).rgb);
	float lumaUpRight 	= rgb2luma(textureLodOffset(sampler2D(screenTexture, sClampLinear), uv, 0.0,ivec2( 1, 1)).rgb);
	float lumaUpLeft 	= rgb2luma(textureLodOffset(sampler2D(screenTexture, sClampLinear), uv, 0.0,ivec2(-1, 1)).rgb);
	float lumaDownRight = rgb2luma(textureLodOffset(sampler2D(screenTexture, sClampLinear), uv, 0.0,ivec2( 1,-1)).rgb);
	
	// Combine the four edges lumas (using intermediary variables for future computations with the same values).
	float lumaDownUp = lumaDown + lumaUp;
	float lumaLeftRight = lumaLeft + lumaRight;
	
	// Same for corners
	float lumaLeftCorners = lumaDownLeft + lumaUpLeft;
	float lumaDownCorners = lumaDownLeft + lumaDownRight;
	float lumaRightCorners = lumaDownRight + lumaUpRight;
	float lumaUpCorners = lumaUpRight + lumaUpLeft;
	
	// Compute an estimation of the gradient along the horizontal and vertical axis.
	float edgeHorizontal =	abs(-2.0 * lumaLeft + lumaLeftCorners)	+ abs(-2.0 * lumaCenter + lumaDownUp ) * 2.0	+ abs(-2.0 * lumaRight + lumaRightCorners);
	float edgeVertical =	abs(-2.0 * lumaUp + lumaUpCorners)		+ abs(-2.0 * lumaCenter + lumaLeftRight) * 2.0	+ abs(-2.0 * lumaDown + lumaDownCorners);
	
	// Is the local edge horizontal or vertical ?
	bool isHorizontal = (edgeHorizontal >= edgeVertical);
	
	// Choose the step size (one pixel) accordingly.
	float stepLength = isHorizontal ? inverseScreenSize.y : inverseScreenSize.x;
	
	// Select the two neighboring texels lumas in the opposite direction to the local edge.
	float luma1 = isHorizontal ? lumaDown : lumaLeft;
	float luma2 = isHorizontal ? lumaUp : lumaRight;
	// Compute gradients in this direction.
	float gradient1 = luma1 - lumaCenter;
	float gradient2 = luma2 - lumaCenter;
	
	// Which direction is the steepest ?
	bool is1Steepest = abs(gradient1) >= abs(gradient2);
	
	// Gradient in the corresponding direction, normalized.
	float gradientScaled = 0.25*max(abs(gradient1),abs(gradient2));
	
	// Average luma in the correct direction.
	float lumaLocalAverage = 0.0;
	if(is1Steepest){
		// Switch the direction
		stepLength = - stepLength;
		lumaLocalAverage = 0.5*(luma1 + lumaCenter);
	} else {
		lumaLocalAverage = 0.5*(luma2 + lumaCenter);
	}
	
	// Shift UV in the correct direction by half a pixel.
	vec2 currentUv = uv;
	if(isHorizontal){
		currentUv.y += stepLength * 0.5;
	} else {
		currentUv.x += stepLength * 0.5;
	}
	
	// Compute offset (for each iteration step) in the right direction.
	vec2 offset = isHorizontal ? vec2(inverseScreenSize.x,0.0) : vec2(0.0,inverseScreenSize.y);
	// Compute UVs to explore on each side of the edge, orthogonally. The QUALITY allows us to step faster.
	vec2 uv1 = currentUv - offset * QUALITY(0);
	vec2 uv2 = currentUv + offset * QUALITY(0);
	
	// Read the lumas at both current extremities of the exploration segment, and compute the delta wrt to the local average luma.
	float lumaEnd1 = rgb2luma(textureLod(sampler2D(screenTexture, sClampLinear), uv1, 0.0).rgb);
	float lumaEnd2 = rgb2luma(textureLod(sampler2D(screenTexture, sClampLinear), uv2, 0.0).rgb);
	lumaEnd1 -= lumaLocalAverage;
	lumaEnd2 -= lumaLocalAverage;
	
	// If the luma deltas at the current extremities is larger than the local gradient, we have reached the side of the edge.
	bool reached1 = abs(lumaEnd1) >= gradientScaled;
	bool reached2 = abs(lumaEnd2) >= gradientScaled;
	bool reachedBoth = reached1 && reached2;
	
	// If the side is not reached, we continue to explore in this direction.
	if(!reached1){
		uv1 -= offset * QUALITY(1);
	}
	if(!reached2){
		uv2 += offset * QUALITY(1);
	}
	
	// If both sides have not been reached, continue to explore.
	if(!reachedBoth){
		
		for(int i = 2; i < ITERATIONS; i++){
			// If needed, read luma in 1st direction, compute delta.
			if(!reached1){
				lumaEnd1 = rgb2luma(textureLod(sampler2D(screenTexture, sClampLinear), uv1, 0.0).rgb);
				lumaEnd1 = lumaEnd1 - lumaLocalAverage;
			}
			// If needed, read luma in opposite direction, compute delta.
			if(!reached2){
				lumaEnd2 = rgb2luma(textureLod(sampler2D(screenTexture, sClampLinear), uv2, 0.0).rgb);
				lumaEnd2 = lumaEnd2 - lumaLocalAverage;
			}
			// If the luma deltas at the current extremities is larger than the local gradient, we have reached the side of the edge.
			reached1 = abs(lumaEnd1) >= gradientScaled;
			reached2 = abs(lumaEnd2) >= gradientScaled;
			reachedBoth = reached1 && reached2;
			
			// If the side is not reached, we continue to explore in this direction, with a variable quality.
			if(!reached1){
				uv1 -= offset * QUALITY(i);
			}
			if(!reached2){
				uv2 += offset * QUALITY(i);
			}
			
			// If both sides have been reached, stop the exploration.
			if(reachedBoth){ break;}
		}
		
	}
	
	// Compute the distances to each side edge of the edge (!).
	float distance1 = isHorizontal ? (uv.x - uv1.x) : (uv.y - uv1.y);
	float distance2 = isHorizontal ? (uv2.x - uv.x) : (uv2.y - uv.y);
	
	// In which direction is the side of the edge closer ?
	bool isDirection1 = distance1 < distance2;
	float distanceFinal = min(distance1, distance2);
	
	// Thickness of the edge.
	float edgeThickness = (distance1 + distance2);
	
	// Is the luma at center smaller than the local average ?
	bool isLumaCenterSmaller = lumaCenter < lumaLocalAverage;
	
	// If the luma at center is smaller than at its neighbour, the delta luma at each end should be positive (same variation).
	bool correctVariation1 = (lumaEnd1 < 0.0) != isLumaCenterSmaller;
	bool correctVariation2 = (lumaEnd2 < 0.0) != isLumaCenterSmaller;
	
	// Only keep the result in the direction of the closer side of the edge.
	bool correctVariation = isDirection1 ? correctVariation1 : correctVariation2;
	
	// UV offset: read in the direction of the closest side of the edge.
	float pixelOffset = - distanceFinal / edgeThickness + 0.5;
	
	// If the luma variation is incorrect, do not offset.
	float finalOffset = correctVariation ? pixelOffset : 0.0;
	
	// Sub-pixel shifting
	// Full weighted average of the luma over the 3x3 neighborhood.
	float lumaAverage = (1.0/12.0) * (2.0 * (lumaDownUp + lumaLeftRight) + lumaLeftCorners + lumaRightCorners);
	// Ratio of the delta between the global average and the center luma, over the luma range in the 3x3 neighborhood.
	float subPixelOffset1 = clamp(abs(lumaAverage - lumaCenter)/lumaRange,0.0,1.0);
	float subPixelOffset2 = (-2.0 * subPixelOffset1 + 3.0) * subPixelOffset1 * subPixelOffset1;
	// Compute a sub-pixel offset based on this delta.
	float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * SUBPIXEL_QUALITY;
	
	// Pick the biggest of the two offsets.
	finalOffset = max(finalOffset,subPixelOffsetFinal);
	
	// Compute the final UV coordinates.
	vec2 finalUv = uv;
	if(isHorizontal){
		finalUv.y += finalOffset * stepLength;
	} else {
		finalUv.x += finalOffset * stepLength;
	}
	
	// Read the color at the new UV coordinates, and use it.
	vec3 finalColor = textureLod(sampler2D(screenTexture, sClampLinear), finalUv, 0.0).rgb;
	return finalColor;
}
*/

#endif