#version 440

uniform sampler2D blurUnit;
uniform sampler2D sceneUnit;
uniform float timeCoef;
uniform float time;
uniform int end;

in vec2 texCoordV;

out vec4 outColor;

void main() {
    float f = (int(time * timeCoef) % 1000)/1000.0;
    if (end == 0)
        outColor = vec4(texture(sceneUnit,texCoordV).rgb, 1.0);
    else
        outColor = vec4(texture(blurUnit,texCoordV).rgb * (f) + texture(sceneUnit,texCoordV).rgb * (1-f), 1.0);
}