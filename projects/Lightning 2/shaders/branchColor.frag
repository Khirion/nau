#version 440

uniform vec4 emission;

out vec4 branchColor;

void main()
{
	branchColor = vec4(emission.rgb, 1.0);
}