#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out VS_OUT
{
    vec3 position;
    vec3 normal;
    vec2 texCoords;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(position, 1.0);
    vs_out.position = worldPos.xyz;
    vs_out.texCoords = texCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.normal = normalMatrix * normal;

    gl_Position = projection * view * worldPos;
}