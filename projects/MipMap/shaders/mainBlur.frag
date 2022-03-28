#version 440

uniform writeonly image2D mainBlur;
uniform writeonly image2D vBlur;
uniform sampler2D texUnit;

in vec2 texCoordV;

uniform float weight[5] = float[] (0.293248, 0.223943, 0.099695, 0.025844, 0.003894);

void main() {
	vec2 tex_offset = 1.0 / textureSize(texUnit, 0); // gets size of single texel
    vec3 result = vec3(0.f, 0.f, 0.f);

    for(int k = -4; k < 5; k++){
        for(int i = -4; i < 5; i++){
                result += texture(texUnit, texCoordV + vec2(tex_offset.x * i, tex_offset.y * k)).rgb * weight[abs(i)] * weight[abs(k)];
        }
    }
    
	imageStore(mainBlur, ivec2(texCoordV * 800), vec4(result, 1.0));
    imageStore(vBlur, ivec2(texCoordV * 800), vec4(result, 1.0));
} 