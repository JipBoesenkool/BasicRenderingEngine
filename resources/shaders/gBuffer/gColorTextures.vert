#version 330 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoords;

out VS_OUT
{
    vec2 texCoords;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(position, 1.0);
    gl_Position = projection * view * worldPos;

    vs_out.texCoords = texCoords;
}