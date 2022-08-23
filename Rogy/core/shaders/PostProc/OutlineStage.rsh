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

uniform sampler2D HighlightTex;
   
void main()
{     
	vec3 color;

   /* float normCol = texture(HighlightTex, TexCoords).r;
    float offsetCol1 = texture(HighlightTex, TexCoords - vec2(0.000, 0.002)).r;
    float offsetCol2 = texture(HighlightTex, TexCoords - vec2(0.002, 0.000)).r;
    float offsetCol3 = texture(HighlightTex, TexCoords - vec2(0.000, -0.002)).r;
    float offsetCol4= texture(HighlightTex, TexCoords - vec2(-0.002, 0.000)).r;

    if((normCol - offsetCol1) > 0.1 || (normCol - offsetCol2) > 0.1 || (normCol - offsetCol3) > 0.1 || (normCol - offsetCol4) > 0.1)
    {
        FragColor = vec4(0.8,0.7,0.001, 1.0);
        return;
    }
*/
   // float thickness = 0.0016;
	float normCol = texture(HighlightTex, TexCoords).r;
    float offsetCol1 = texture(HighlightTex, TexCoords - vec2(0.0015, 0.000)).r;
    float offsetCol2 = texture(HighlightTex, TexCoords - vec2(0.000, 0.0015)).r;

    if(abs(normCol - offsetCol1) > 0.05 || abs(normCol - offsetCol2) > 0.05 )
    {
        //FragColor = vec4(0.7,0.6,0.001, 1.0);
        FragColor = vec4(0.922,0.366,0.0015, 1.0);
        return;
    }

    discard;
	//FragColor = vec4(0.0, 0.0, 0.0, 1.0);
} 

#endif