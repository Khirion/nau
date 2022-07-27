#version 440

uniform writeonly image2D mainBlur;
uniform sampler2D texUnit;

in vec2 texCoordV;

void main() {
	vec2 tex_offset = 1.0 / textureSize(texUnit, 0); // gets size of single texel
    vec3 result = texture(texUnit, texCoordV).rgb;

    for(int k = -1; k < 2; k++){
        for(int i = -1; i < 2; i++){
                result += texture(texUnit, texCoordV + vec2(tex_offset.x * i, tex_offset.y * k)).rgb;
        }
    }

	imageStore(mainBlur, ivec2(texCoordV * 800), vec4(result, 1.0));
} 