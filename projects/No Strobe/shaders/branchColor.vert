#version 440

uniform	mat4 m_pvm;

in vec4 position;	// local space

void main () {
	gl_Position = m_pvm * position;	
}