#version 440

uniform	mat4 m_pvm;
uniform mat4 m_view;
uniform	mat3 m_normal;
uniform vec4 l_dir0; // global space

in vec4 position;	// local space
in vec3 normal0;		// local space

out	vec3 normal;
out	vec3 l_dir;

void main () {
	normal = normalize(m_normal * normal0);
	l_dir = normalize(vec3(m_view * -l_dir0));

	gl_Position = m_pvm * position;	
}