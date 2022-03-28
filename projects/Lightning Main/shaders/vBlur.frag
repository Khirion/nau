#version 440

uniform writeonly image2D imageUnit;
uniform sampler2D texUnit;

in vec2 texCoordV;

uniform float weight[13] = float[] (0.099654, 0.096603, 0.088001, 0.075333, 0.0606, 0.04581, 0.032542, 0.021724, 0.013627, 0.008033, 0.00445, 0.002316, 0.001133);

void main() {
	vec2 tex_offset = 1.0 / textureSize(texUnit, 0); // gets size of single texel
    vec3 result = texture(texUnit, texCoordV).rgb * weight[0];

    for(int i = 1; i < 13; i++){
            result += texture(texUnit, texCoordV + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(texUnit, texCoordV - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
	imageStore(imageUnit, ivec2(texCoordV * 1325), vec4(result, 1.0));
} 