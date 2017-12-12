#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoords;

out vec2 TexCoords;

uniform mat4 model;

void main()
{
	gl_Position = model * vec4(position, 0.0f, 1.0f);
	// We swap the y-axis by substracing our coordinates from 1. This is done because most images have the top y-axis inversed with OpenGL's top y-axis.
	TexCoords = vec2(texCoords.x, texCoords.y);
}