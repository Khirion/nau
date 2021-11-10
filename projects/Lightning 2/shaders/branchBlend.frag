#version 440

uniform sampler2D blurUnit;
uniform sampler2D sceneUnit;
uniform int control;
uniform float timeCoef;
uniform float time;
uniform float stepTime = 0.2;

in vec2 texCoordV;

out vec4 outColor;

void main() {
    float f = (time - (stepTime * (control+1)));

    switch (control){
        case -1:
            outColor = vec4(texture(sceneUnit,texCoordV).rgb * 0.5, 1.0);
            break;

        case 0:
            f *= 2.5;
            outColor = vec4(texture(blurUnit,texCoordV).rgb * (f) + texture(sceneUnit,texCoordV).rgb * (0.5), 1.0);
            break;
        
        case 1:
            f *= 2.5;
            outColor = vec4(texture(blurUnit,texCoordV).rgb * (f) + texture(sceneUnit,texCoordV).rgb * (0.5), 1.0);
            break;
        
        case 2:
            f *= 2.5;
            outColor = vec4(texture(blurUnit,texCoordV).rgb * (f) + texture(sceneUnit,texCoordV).rgb * (0.5), 1.0);
            break;
        
        case 3:
            f *= 2.5;
            outColor = vec4(texture(blurUnit,texCoordV).rgb * (f) + texture(sceneUnit,texCoordV).rgb * (0.5), 1.0);
            break;
        
        case 4:
            f *= 2.5;
            outColor = vec4(texture(blurUnit,texCoordV).rgb * (f) + texture(sceneUnit,texCoordV).rgb * (0.5), 1.0);
            break;
            
        case 5:
            f *= 2.5;
            outColor = vec4(texture(blurUnit,texCoordV).rgb * (f) + texture(sceneUnit,texCoordV).rgb * (0.5), 1.0);
            break;
    };
}