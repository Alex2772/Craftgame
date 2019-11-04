#version 330 core

in vec4 position;
in vec2 uv;
in vec4 color;
in vec3 trans;
in vec4 col;
in vec4 uvs;

out vec4 color_pass;
out vec2 uv_pass;

uniform mat4 VP;
uniform mat4 M;

void main() {
	mat4 x = M;
	x[3] = vec4(trans, 1);
	gl_Position = VP * x * position;
	uv_pass = mix(uvs.xy, uvs.za, uv);
	color_pass = color * col;
}