#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4[100] models;

void main()
{
    mat4 model = models[gl_InstanceID];
    gl_Position = model * vec4(aPos, 1.0);
}