#version 440

uniform	vec4 diffuse;
uniform vec4 emission;

in vec3 normal;
in vec3 l_dir;

out vec4 scene;

void main()
{
	float intensity;
	vec3 l, n;
	
	n = normalize(normal);
	intensity = max(dot(l_dir,n),0.0);
	
	scene = vec4(max((diffuse * 0.25) + emission, (diffuse * intensity) + emission).rgb, 1.0);
}