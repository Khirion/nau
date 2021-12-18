#version 440

uniform vec4 emission;

layout (location = 0) out vec4 scene;

void main()
{
	scene = vec4(1.0, 0.0, 0.0, 1.0);
}