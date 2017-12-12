#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in VS_OUT
{
    vec3 position;
    vec3 normal;
    vec2 texCoords;
    vec3 tangent;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
vec3 CalcNormal()
{
    vec3 normal = normalize(fs_in.normal);
    vec3 tangent= normalize(fs_in.tangent);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 bitangent = cross(tangent, normal);

    vec3 bumpMapNormal = texture(texture_normal1, fs_in.texCoords).xyz;
    bumpMapNormal = 2.0 * bumpMapNormal - vec3(1.0f, 1.0f, 1.0f);

    // TBN must form a right handed coord system.
    // Some models have symetric UVs. Check and fix.

    vec3 newNormal;
    mat3 TBN = mat3(tangent, bitangent, normal);
    newNormal = TBN * bumpMapNormal;
    newNormal = normalize(newNormal);
    return newNormal;
}

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gPosition = fs_in.position;
    // also store the per-fragment normals into the gbuffer
    gNormal = CalcNormal();
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(texture_diffuse1, fs_in.texCoords).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(texture_specular1, fs_in.texCoords).r;
}