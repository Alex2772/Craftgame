#version 330


in vec3 position;
out float l;
uniform mat4 mt;

void main() {
	vec4 pos = mt * vec4(position, 1);
	gl_Position = pos;
	l = length(position);
}