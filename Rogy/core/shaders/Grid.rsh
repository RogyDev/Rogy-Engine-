// --------------------------------------------[ Vertex Shader ]------------------------------------------------------//
#ifdef COMPILING_VS

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

layout (std140) uniform Matrices
{
    mat4 proj;
    mat4 view;
	mat4 VP; // projection * view
};

uniform vec3 pos;

out vec3 nearPoint;
out vec3 farPoint;
out mat4 fragView;
out mat4 fragProj;

// Grid position are in xy clipped space
vec3 gridPlane[6] = vec3[](
    vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection) {
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

// normal vertice projection
void main() 
{
    //gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
    //gl_Position = proj * view * vec4(aPos.x, aPos.y, 0.0, 1.0); 
    //gl_Position = proj * view * vec4(gridPlane[gl_VertexID].xyz, 1.0);

    fragView = view;
    fragProj = proj;
    vec3 p = vec3(aPos.x, aPos.y, 0.0); //gridPlane[gl_VertexIndex].xyz;
    nearPoint = UnprojectPoint(p.x, p.y, 0.0, view, proj).xyz; // unprojecting on the near plane
    farPoint = UnprojectPoint(p.x, p.y, 1.0, view, proj).xyz; // unprojecting on the far plane
    gl_Position = vec4(p, 1.0); // using directly the clipped coordinates
}

#endif

// -------------------------------------------[ Fragment Shader ]------------------------------------------------------//
#ifdef COMPILING_FS

out vec4 FragColor;

in vec3 nearPoint;
in vec3 farPoint;
in mat4 fragView;
in mat4 fragProj;

layout (std140) uniform Matrices
{
    mat4 proj;
    mat4 view;
	mat4 VP; // projection * view
};

vec4 grid(vec3 fragPos3D, float scale, bool drawAxis) {
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    // z axis
    if(fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
        color.z = 1.0;
    // x axis
    if(fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
        color.x = 1.0;
    return color;
}

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = VP * vec4(pos, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}
float computeLinearDepth(vec3 pos) {
    float far = 150;
    float near = 0.01;
    vec4 clip_space_pos = VP * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near)); // get linear value between 0.01 and 100
    return linearDepth / far; // normalize
}
void main()
{     
   /* float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);
    gl_FragDepth = computeDepth(fragPos3D);
    //if(t < 0) discard;
   // FragColor =  grid(fragPos3D, 10, true);//vec4(1.0, 0.0, 0.0, 1.0);
    FragColor = grid(fragPos3D, 1, true) * float(t > 0);*/

    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    gl_FragDepth = computeDepth(fragPos3D);

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.5 - linearDepth));

    FragColor = (/*grid(fragPos3D, 5, true) + */grid(fragPos3D, 0.5, true))* float(t > 0); // adding multiple resolution for the grid
    FragColor.a *= fading;
} 

/*
// Apply the grid.
vec3 applyGrid(vec3 colour, vec2 uvs, mat4 invVP, mat4 vP)
{
  uvs = 2.0 * uvs - 1.0.xx;
  vec3 nearPoint = unProject(vec3(uvs, 0.0), invVP);
  vec3 farPoint = unProject(vec3(uvs, 1.0), invVP);
  float t = -nearPoint.y / (farPoint.y - nearPoint.y);
  float s = -nearPoint.z / (farPoint.z - nearPoint.z);

  vec3 xzFragPos3D = nearPoint + t * (farPoint - nearPoint);
  vec3 xyFragPos3D = nearPoint + s * (farPoint - nearPoint);

  // Compute the depth of the current fragment along both the x-y and x-z planes
  // [0, 1].
  vec4 xzFragClipPos = vP * vec4(xzFragPos3D, 1.0);
  float xzFragDepth = 0.5 * (xzFragClipPos.z / xzFragClipPos.w) + 0.5;

  // Perform a depth test so the grid doesn't draw over scene objects.
 // float depth = texture(gDepth, 0.5 * uvs + 0.5.xx).r;
  //float xzDepthTest = float(depth > xzFragDepth);

  // Apply the grid within 10 units.
  float aabb = float(abs(xzFragPos3D.x) <= 10.0 && abs(xzFragPos3D.z) <= 10.0);
  vec4 gridColour = grid(xzFragPos3D, 1.0) * float(t > 0.0);

  return mix(colour, gridColour.rgb, aabb * gridColour.a);
}

*/


#endif