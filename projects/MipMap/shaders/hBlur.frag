#version 440

uniform writeonly image2D imageUnit;
uniform sampler2D texUnit;

in vec2 texCoordV;

uniform float weight[26] = float[] (0.047969, 0.047625, 0.046608, 0.044961, 0.042752, 0.04007, 0.03702, 0.033714, 0.030264, 0.026778, 0.023356, 0.02008, 0.017016, 0.014214, 0.011704, 0.009499, 0.0076, 0.005993, 0.004658, 0.003569, 0.002696, 0.002007, 0.001473, 0.001065, 0.00076, 0.000534);

void main() {
	vec2 tex_offset = 1.0 / textureSize(texUnit, 0); // gets size of single texel
    vec3 result = texture(texUnit, texCoordV).rgb * weight[0];

    for(int i = 1; i < 26; i++){
            result += texture(texUnit, texCoordV + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(texUnit, texCoordV - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
	imageStore(imageUnit, ivec2(texCoordV * 800), vec4(result, 1.0));
} 