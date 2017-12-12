#version 330 core
layout (location = 1) out vec3 gNormal;

in VS_OUT
{
    vec3 normal;
    vec2 texCoords;
    vec3 tangent;
} fs_in;

uniform sampler2D texture_normal1;

vec3 CalcNormal()
{
    vec3 normal = normalize(fs_in.normal);
    vec3 tangent= normalize(fs_in.tangent);
    tangent = normalize(tangent - dot(tangent, normal) * normal);

    vec3 bitangent = cross(tangent, normal);

    vec3 bumpMapNormal = texture(texture_normal1, fs_in.texCoords).xyz;
    bumpMapNormal = 2.0 * bumpMapNormal - vec3(1.0f, 1.0f, 1.0f);

    vec3 newNormal;
    mat3 TBN = mat3(tangent, bitangent, normal);
    newNormal = TBN * bumpMapNormal;
    newNormal = normalize(newNormal);
    return newNormal;
}

void main()
{
    // also store the per-fragment normals into the gbuffer
    gNormal = CalcNormal();
}