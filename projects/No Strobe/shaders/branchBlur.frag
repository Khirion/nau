#version 440

uniform writeonly image2D imageUnit;
uniform sampler2D texUnit;

in vec2 texCoordV;

uniform float weight[3] = float[] (0.551379, 0.212864, 0.011447);

void main() {
	vec2 tex_offset = 1.0 / textureSize(texUnit, 0); // gets size of single texel
    vec3 result = vec3(0.f, 0.f, 0.f);

    for(int k = -2; k < 3; k++){
        for(int i = -2; i < 3; i++){
                result += texture(texUnit, texCoordV + vec2(tex_offset.x * i, tex_offset.y * k)).rgb * weight[abs(i)] * weight[abs(k)];
        }
    }
	imageStore(imageUnit, ivec2(texCoordV * 1325), vec4(result, 1.0));
} 