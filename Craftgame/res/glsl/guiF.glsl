#version 330

in vec3 pass_Pos;
in vec2 pass_texCoords;
out vec4 color;

uniform vec2 uv;
uniform vec2 uv2;
uniform vec4 c;
uniform int texturing;
uniform sampler2D tex;
uniform sampler2D mask;
uniform int useProjection;
uniform mat4 projection;
uniform int masking;
uniform vec4 colors[4];

#define DENSITY 1
#define GRADIENT 2.5

void main() {
	if (texturing == 1) {
		vec2 tuv;
		tuv.x = mix(uv.x, uv2.x, pass_texCoords.x);
		tuv.y = mix(uv.y, uv2.y, pass_texCoords.y);
		color = texture(tex, tuv) * c;
	}
	else if (texturing == 2) {
		vec2 tuv;
		tuv.x = mix(uv.x, uv2.x, pass_texCoords.x);
		tuv.y = mix(uv.y, uv2.y, pass_texCoords.y);
		color = vec4(c.x, c.y, c.z, c.a * texture(tex, tuv).r);
	}
	else if (texturing == 3) {
		vec4 color1 = mix(colors[0], colors[1], pass_texCoords.x);
		vec4 color2 = mix(colors[2], colors[3], pass_texCoords.x);
		vec4 co = mix(color1, color2, pass_texCoords.y);
		color = co * c;
	}
	else if (texturing == 4) {
		float Xyi = (cos((pass_texCoords.x + 1) / 3.14f * 10) + 1) / 2;
		float Yopta = (cos((pass_texCoords.y + 1) / 3.14f * 10) + 1) / 2;
		vec4 color1 = mix(colors[0], colors[1], Xyi);
		vec4 color2 = mix(colors[2], colors[3], Xyi);
		vec4 co = mix(color1, color2, Yopta);
		color = co * c;
	}
	else {
		color = c;
	}
	if (masking == 1)
		color.a *= texture(mask, (pass_Pos.xy + 1) / 2).r;
}