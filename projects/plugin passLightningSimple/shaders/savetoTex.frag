#version 440

uniform vec4 emission;

out vec4 colorOut;

void main(){
    colorOut = emission;
}