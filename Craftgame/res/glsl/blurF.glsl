#version 330

#define KERNEL_SIZE 14

in vec3 pass_Pos;
in vec2 pass_texCoords;
out vec4 color;

uniform float orientation;
uniform sampler2D fb;
uniform vec2 scales;
uniform int kSize;
uniform float kernel[KERNEL_SIZE * 2 + 1];
uniform vec4 bounds;
uniform vec4 c;

void main() {
	vec2 scr = (pass_Pos.xy + 1.0) / 2.0;
	vec4 sum = vec4(0, 0, 0, 0);
	if (orientation == 0.f) {
		for (int i = -kSize; i <= kSize; i++) {
			vec2 cd = vec2(scr.x + i * scales.x, scr.y);
			vec4 t = texture(fb, cd);
			sum += t * kernel[i + kSize];
		
		}
	}
	else {
		for (int i = -kSize; i <= kSize; i++) {
			vec2 cd = vec2(scr.x, scr.y + i * scales.y);
			vec4 t = texture(fb, cd);
			sum += t * kernel[i + kSize];
		}
	}
	color = sum * c;
}