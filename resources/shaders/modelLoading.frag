#version 330 core
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

out vec4 color;

void main()
{
    color = texture(texture_diffuse1, TexCoords);
}