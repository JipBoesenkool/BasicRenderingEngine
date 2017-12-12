#version 330 core
layout (location = 2) out vec4 gAlbedoSpec;

in VS_OUT
{
    vec3 color;
} fs_in;

void main()
{
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = color.rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    //TODO: default specular?
    gAlbedoSpec.a = 0;
}