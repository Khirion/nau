#version 440

uniform vec4 emission;

layout (location = 0) out vec4 scene;
layout (location = 1) out vec4 mainColor;

void main()
{
	scene = mainColor = vec4(emission.rgb, 1.0);
}