#version 330 core
layout (location = 1) out vec3 gNormal;

in VS_OUT
{
    vec3 normal;
} fs_in;

void main()
{
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(fs_in.normal);
}