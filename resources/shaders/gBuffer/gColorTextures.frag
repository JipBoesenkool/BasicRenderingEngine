#version 330 core
layout (location = 2) out vec4 gAlbedoSpec;

in VS_OUT
{
    vec2 texCoords;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
void main()
{
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(texture_diffuse1, fs_in.texCoords).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(texture_specular1, fs_in.texCoords).r;
}