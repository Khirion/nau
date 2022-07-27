#version 440

uniform vec4 emission;

layout (location = 0) out vec4 scene;
layout (location = 2) out vec4 branchColor;

void main()
{
	scene = branchColor = vec4(emission.rgb, 1.0);
}