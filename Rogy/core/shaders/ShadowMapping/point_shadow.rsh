// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS
layout (location = 0) in vec3 aPos;

uniform mat4 models[100];

void main()
{
    mat4 model = models[gl_InstanceID];
    gl_Position = model * vec4(aPos, 1.0);
}

#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS

in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    float lightDistance = length(FragPos.xyz - lightPos);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}
#endif

// -------------------------------------------[ Geometry Shader ]------------------------------------------------------//
#ifdef COMPILING_GS

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
} 
#endif