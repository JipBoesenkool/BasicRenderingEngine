#version 330 core
layout (location = 0) out vec3 gPosition;

in VS_OUT
{
    vec3 position;
} fs_in;

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gPosition = fs_in.position;
}