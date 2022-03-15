#version 440

uniform writeonly image2D imageUnit;
uniform sampler2D texUnit;

in vec2 texCoordV;

uniform float weight[25] = float[] (0.049945, 0.049557, 0.04841, 0.046558, 0.044083, 0.041094, 0.037714, 0.034076, 0.030313, 0.026547, 0.02289, 0.01943, 0.016239, 0.013361, 0.010823, 0.008631, 0.006777, 0.005239, 0.003987, 0.002987, 0.002203, 0.0016, 0.001144, 0.000805, 0.000558);

void main() {
	vec2 tex_offset = 1.0 / textureSize(texUnit, 0); // gets size of single texel
    vec3 result = texture(texUnit, texCoordV).rgb * weight[0];

    for(int i = 1; i < 25; i++){
            result += texture(texUnit, texCoordV + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(texUnit, texCoordV - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
	imageStore(imageUnit, ivec2(texCoordV * 1325), vec4(result, 1.0));
} 