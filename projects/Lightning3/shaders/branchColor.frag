#version 440

uniform vec4 emission;

layout (location = 0) out vec4 scene;

void main()
{
	scene = vec4(0.5, 0.5, 0.5, 1.0);
}