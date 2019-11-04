#version 330

in vec3 pass_Pos;
in vec2 pass_texCoords;
in vec4 pass_Color;
out vec4 color;

uniform vec2 uv;
uniform vec2 uv2;
uniform vec4 c;
uniform sampler2D tex;
uniform sampler2D mask;
uniform int masking;
uniform int texturing;
uniform vec4 colors[4];

#define DENSITY 1
#define GRADIENT 2.5

void main() {
	if (texturing == 1) {
		vec2 tuv;
		tuv.x = mix(uv.x, uv2.x, pass_texCoords.x);
		tuv.y = mix(uv.y, uv2.y, pass_texCoords.y);
		vec4 ac = texture(tex, tuv);
		color = ac * c;
	}
	else if (texturing == 2) {
		vec4 fc = pass_Color * c;
		color = vec4(fc.x, fc.y, fc.z, fc.a * texture(tex, pass_texCoords).r);
		//color = vec4(pass_texCoords, 0, 1);
	}
	else if (texturing == 3) {
		vec4 color1 = mix(colors[0], colors[1], pass_texCoords.x);
		vec4 color2 = mix(colors[2], colors[3], pass_texCoords.x);
		vec4 co = mix(color1, color2, pass_texCoords.y);
		color = co * c;
	}
	else if (texturing == 4) {
		float X = (cos((pass_texCoords.x + 1) / 3.14f * 10) + 1) / 2;
		float Y = (cos((pass_texCoords.y + 1) / 3.14f * 10) + 1) / 2;
		vec4 color1 = mix(colors[0], colors[1], X);
		vec4 color2 = mix(colors[2], colors[3], Y);
		vec4 co = mix(color1, color2, Y);
		color = co * c;
	}
	else {
		color = c;
	}
	if (masking == 1)
		color.a *= texture(mask, (pass_Pos.xy + 1) / 2).r;
}