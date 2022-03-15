#version 440

uniform	vec4 diffuse;
uniform vec4 emission;

in vec3 normal;
in vec3 l_dir;

layout (location = 0) out vec4 scene;
layout (location = 1) out vec4 mainColor;
layout (location = 2) out vec4 branchColor;

void main()
{
	float intensity;
	vec3 l, n;
	
	n = normalize(normal);
	intensity = max(dot(l_dir,n),0.0);
	
	scene = vec4(max((diffuse * 0.25) + emission, (diffuse * intensity) + emission).rgb, 1.0);
	mainColor = vec4(0,0,0,0);
	branchColor = vec4(0,0,0,0);
}