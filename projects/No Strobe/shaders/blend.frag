#version 440

uniform sampler2D mainColor;
uniform sampler2D branchColor;
uniform sampler2D mainBlur;
uniform sampler2D branchBlur;
uniform sampler2D blur;
uniform sampler2D scene;
uniform int control;

in vec2 texCoordV;

out vec4 outColor;

void main() {
    outColor = vec4(max(texture(blur,texCoordV).rgb + texture(mainBlur,texCoordV).rgb * 4, texture(branchBlur, texCoordV).rgb) + textureLod(blur, texCoordV, 7.75).rgb * 10 * vec3(0.3, 0.5, 1.0), 1.0);
}