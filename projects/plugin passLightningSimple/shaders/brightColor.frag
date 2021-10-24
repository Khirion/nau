#version 440

uniform	vec4 diffuse;
uniform vec4 emission;

in vec3 normal;
in vec3 l_dir;

layout (location = 0) out vec4 scene;
layout (location = 1) out vec4 brightColor;

void main()
{
	float intensity;
	vec3 l, n;
	
	n = normalize(normal);
	// no need to normalize the light direction!
	intensity = max(dot(l_dir,n),0.0);
	
	scene = vec4(max((diffuse * 0.25) + emission, (diffuse * intensity) + emission).rgb, 1.0);
	brightColor = vec4(emission.rgb, 1.0);
}