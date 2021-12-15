#version 440

uniform writeonly image2D imageUnit;
uniform sampler2D texUnit;

in vec2 texCoordV;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
	vec2 tex_offset = 1.0 / textureSize(texUnit, 0); // gets size of single texel
    vec3 result = -(texture(texUnit, texCoordV).rgb * weight[0]); // current fragment's contribution

    for(int k = 0; k < 5; k++)
        {
        for(int i = 0; i < 5; i++)
            {
                result += texture(texUnit, texCoordV + vec2(tex_offset.x * i, tex_offset.y * k)).rgb * weight[max(i,k)];
                result += texture(texUnit, texCoordV - vec2(tex_offset.x * i, tex_offset.y * k)).rgb * weight[max(i,k)];
            }
        }  
	imageStore(imageUnit, ivec2(texCoordV * 1024), vec4(result, 1.0));
} 