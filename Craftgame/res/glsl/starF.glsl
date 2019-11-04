#version 330

in float l;
out vec4 c;

uniform float str;

void main() {
	c = vec4(str, str, str, (l + 0.7f) / 1.7f);
	gl_FragDepth = 0.999998f;
}