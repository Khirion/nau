#version 440

uniform sampler2D mainColor;
uniform sampler2D branchColor;
uniform sampler2D mainBlur;
uniform sampler2D branchBlur;
uniform sampler2D blur;
uniform int control;
uniform float timeCoef;
uniform float time;
uniform float stepTime = 0.2;

in vec2 texCoordV;

out vec4 outColor;

void main() {
    float f = (time - (stepTime * (control)));
    outColor = vec4(max(texture(mainColor,texCoordV).rgb, texture(branchColor, texCoordV).rgb * 0.25), 1.0);
}