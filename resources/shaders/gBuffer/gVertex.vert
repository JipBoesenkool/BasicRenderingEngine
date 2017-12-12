#version 330 core
layout (location = 0) in vec3 position;

out VS_OUT
{
    vec3 position;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(position, 1.0);
    vs_out.position = worldPos.xyz;
    gl_Position = projection * view * worldPos;
}