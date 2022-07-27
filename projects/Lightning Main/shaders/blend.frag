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
            outColor = vec4(max(texture(blur,texCoordV).rgb * 0.5 + texture(mainBlur,texCoordV).rgb * 2, texture(branchBlur, texCoordV).rgb) + textureLod(blur, texCoordV, 7.75).rgb * 12 * vec3(0.00, 0.30, 0.60), 1.0)* (1-f);
            break;
        
        case 2:
            f *= 3;
            outColor = vec4(max(texture(blur,texCoordV).rgb * 0.5 + texture(mainBlur,texCoordV).rgb * 2, texture(branchBlur, texCoordV).rgb) + textureLod(blur, texCoordV, 7.75).rgb * 12 * vec3(0.00, 0.30, 0.60), 1.0) * (f+0.4);
            break;
        
        case 3:
            f *= 3;
            outColor = vec4(max(texture(blur,texCoordV).rgb * 0.5 + texture(mainBlur,texCoordV).rgb * 2, texture(branchBlur, texCoordV).rgb) + textureLod(blur, texCoordV, 7.75).rgb * 12 * vec3(0.00, 0.30, 0.60), 1.0) * (1-f);
            break;
        
        case 4:
            f *= 3;
            outColor = vec4(max(texture(blur,texCoordV).rgb * 0.5 + texture(mainBlur,texCoordV).rgb * 2, texture(branchBlur, texCoordV).rgb) + textureLod(blur, texCoordV, 7.75).rgb * 12 * vec3(0.00, 0.30, 0.60), 1.0) * (f+0.4);
            break;
            
        case 5:
            f *= 5;
            outColor = vec4(max(texture(blur,texCoordV).rgb * 0.5 + texture(mainBlur,texCoordV).rgb * 2, texture(branchBlur, texCoordV).rgb) + textureLod(blur, texCoordV, 7.75).rgb * 12 * vec3(0.00, 0.30, 0.60), 1.0) * (1-(f*5));
            break;
    };

    if (control > 0)
        outColor *= 2;
}