#version 330
in vec4 position;
in vec2 texCoords;

out vec2 pass_uv;
uniform mat4 tr;

void main() {
	gl_Position = tr * position;
	pass_uv = texCoords;
}