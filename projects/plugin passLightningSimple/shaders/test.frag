#version 440

uniform writeonly image2D imageUnit;
uniform	vec4 diffuse;
uniform	vec4 emission;

in Data{
	vec3 normal;
	vec3 l_dir;
};

out vec4 outputF;

void main()
{
	float intensity;
	vec3 l, n;
	
	n = normalize(normal);
	// no need to normalize the light direction!
	intensity = max(dot(l_dir,n),0.0);
	
	outputF = max((diffuse * 0.25) + emission, (diffuse * intensity) + emission);
	outputF.a = 1.0;
	imageStore(imageUnit, ivec2(gl_FragCoord), vec4(0.0, 1.0, 0.0, 1.0));a
}