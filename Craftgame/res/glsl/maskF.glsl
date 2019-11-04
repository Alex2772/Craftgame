
#version 330

in vec2 pass_texCoords;

uniform vec2 uv;
uniform vec2 uv2;

out vec4 color;
uniform sampler2D t;
uniform int texturing;
uniform vec4 c;

void main() {
	color = c;
	if (texturing != 0) {
		vec2 tuv;
		tuv.x = mix(uv.x, uv2.x, pass_texCoords.x);
		tuv.y = mix(uv.y, uv2.y, pass_texCoords.y);
		color = vec4(color.xyz * texture(t, tuv).a, color.a);
	}
	/*
	vec2 kek = pass_texCoords * 2 - 1;
	float f = cos(kek.x / 3.14 * 5) * cos(kek.y / 3.14 * 5);
	if (f < 0.1) {
		color.xyz = vec3(0, 0, 0);
	}*/
}