#version 440

uniform sampler2D mainColor;
uniform sampler2D branchColor;
uniform sampler2D mainBlur;
uniform sampler2D branchBlur;
uniform sampler2D blur;
uniform sampler2D scene;
uniform int control;
uniform float timeCoef;
uniform float time;
uniform float stepTime = 0.2;

in vec2 texCoordV;

out vec4 outColor;

void main() {
    float f = (time - (stepTime * (control)));

    switch (control){
        case 0:
            outColor = vec4(max(texture(mainColor,texCoordV).rgb, texture(branchColor, texCoordV).rgb * 0.5), 1.0);
            break;
        
        case 1:
            f *= 3;
            outColor = vec4(max((texture(blur,texCoordV).rgb + texture(mainBlur,texCoordV).rgb)*2, texture(branchBlur, texCoordV).rgb) * (1-f), 1.0);
            break;
        
        case 2:
            f *= 3;
            outColor = vec4(max((texture(blur,texCoordV).rgb + texture(mainBlur,texCoordV).rgb)*2, texture(branchBlur, texCoordV).rgb) * (f+0.4), 1.0);
            break;
        
        case 3:
            f *= 3;
            outColor = vec4(max((texture(blur,texCoordV).rgb + texture(mainBlur,texCoordV).rgb)*2, texture(branchBlur, texCoordV).rgb) * (1-f), 1.0);
            break;
        
        case 4:
            f *= 3;
            outColor = vec4(max((texture(blur,texCoordV).rgb + texture(mainBlur,texCoordV).rgb)*2, texture(branchBlur, texCoordV).rgb) * (f+0.4), 1.0);
            break;
            
        case 5:
            f *= 5;
            outColor = vec4(max((texture(blur,texCoordV).rgb + texture(mainBlur,texCoordV).rgb)*2, texture(branchBlur, texCoordV).rgb) * (1-(f*5)), 1.0);
            break;
    };

    if (control > 0)
        outColor *= 2;
}