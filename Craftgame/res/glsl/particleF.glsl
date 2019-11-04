#version 330 core

in vec4 color_pass;
in vec2 uv_pass;

out vec4 color;

uniform sampler2D tex;
uniform vec4 c;

void main(void) {
	color = texture(tex, uv_pass) * color_pass * c;
}
