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

    if (control == 0)
        outColor = vec4(max(texture(mainColor,texCoordV).rgb, texture(branchColor, texCoordV).rgb * 0.5), 1.0);
    else{
        outColor = vec4(max((textureLod(blur, texCoordV, 8).rgb * 7 * vec3(0.0, 0.35, 0.73)+ texture(blur,texCoordV).rgb * 1.5 + texture(mainBlur,texCoordV).rgb * 2), texture(branchBlur, texCoordV).rgb), 1.0) * 2;
    }
}