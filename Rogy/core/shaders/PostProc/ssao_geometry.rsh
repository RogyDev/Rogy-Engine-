
// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform bool invertedNormals;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool sky;

void main()
{
    if(sky)
    {
        FragPos = aPos;
	    mat4 rotView = mat4(mat3(view));
	    vec4 clipPos = projection * rotView * vec4(aPos, 1.0);
	    gl_Position = clipPos.xyww;
        return;
    }

    vec4 viewPos = view * model * vec4(aPos, 1.0);
    FragPos = viewPos.xyz; 
	//FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    Normal = normalMatrix * (invertedNormals ? -aNormal : aNormal);
    //Normal = mat3(transpose(inverse(model))) * aNormal;
	
    gl_Position = projection * viewPos;
}

#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

uniform bool sky;

void main()
{    
    if(sky)
    {
        gPosition = vec3(99999.0, 99999.0, 99999.0);
        return;
    }
    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
	
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
	
//	float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
//	gDepth = vec3(depth);
}

#endif