#version 440

uniform writeonly image2D imageUnit;
uniform	vec4 emission;


in vec2 texCoordV;

void main() {
	ivec2 texCoord = ivec2(texCoordV * 1080);
	imageStore(imageUnit, texCoord, emission);
} 