
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

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;

uniform bool horizontal;
//uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);
//uniform float weight[5] = float[] (0.06136, 	0.24477,	0.38774	,0.24477	,0.06136);
const float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
const float weights[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );
void main()
{
    vec2 texSize = textureSize(image, 0); 
    vec2 tex_offset = (1.0 / texSize); // gets size of single texel
    vec3 result = texture(image, TexCoords).rgb * weights[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 3; ++i)
        {
            result += texture(image, TexCoords + vec2(offset[i] / texSize.x, 0.0)).rgb * weights[i];
            result += texture(image, TexCoords - vec2(offset[i] / texSize.x, 0.0)).rgb * weights[i];
        }
    }
    else
    {
        for(int i = 1; i < 3; ++i)
        {
            result += texture(image, TexCoords + vec2(0.0, offset[i] / texSize.x)).rgb * weights[i];
            result += texture(image, TexCoords - vec2(0.0, offset[i] / texSize.x)).rgb * weights[i];
        }
    }
    FragColor = vec4(result, 1.0);
	/*vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
    vec3 result = texture(image, TexCoords).rgb * weight[0];
	
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
			result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
			result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
	//FragColor = texture(image, TexCoords);
	//return;
	
	/*vec2 tex_offset = 1.0 / (textureSize(image, 0)/3); // gets size of single texel
    vec3 result = texture(image, TexCoords).rgb * weight[0];
    if(horizontal)
    {
        for(int i = 1; i < 4; ++i)
        {
           result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i] * 0.9;
           result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i] * 0.9;
        }
    }
    else
    {
        for(int i = 1; i < 4; ++i)
        {
            result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i] * 0.9;
            result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i] * 0.9;
        }
    }*/
}

#endif