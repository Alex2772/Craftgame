#version 330 core

in vec4 position;
in vec2 uv;
in vec4 color;

out vec4 color_pass;
out vec2 uv_pass;

uniform mat4 VP;

void main() {
	mat4 x = mat4(1.0);
	//x[3] = vec4(trans, 1);
	gl_Position = VP * x * position;
	uv_pass = uv;
	color_pass = color;
}