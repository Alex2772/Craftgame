#version 330

#define LIGHT 16

struct LightSource {
	vec3 pos;
	vec3 color;
};
struct Material {
	sampler2D tex;
	sampler2D normal;
	vec4 color;
	float reflectivity;
	float shineDump;
};

in vec2 pass_texCoords;
in vec3 surfaceNormal;
in vec3 toCamera;
in float visibility;
out vec4 color;

in vec3 tl[LIGHT];
uniform LightSource light[LIGHT];
uniform Material material;
uniform int texturing;
uniform int useNormal;
uniform vec4 c;

void main() {
	if (texturing == 1) {
		color = texture(material.tex, pass_texCoords) * material.color;
	}
	else {
		color = material.color;
	}
	if (color.a < 0.5) {
		discard;
	}
	vec3 unitNormal;
	if (useNormal == 1)
		unitNormal = normalize((2.0 * texture(material.normal, pass_texCoords) - 1.0).rgb);
	else
		unitNormal = normalize(surfaceNormal);
	vec3 specular = vec3(0, 0, 0);
	float ref = material.reflectivity;
	for (int i = 0; i < LIGHT; i++) {
		float distance = length(tl[i]);
		vec3 unitToLight = normalize(tl[i]);
		vec3 c = light[i].color;
		float rangecheck = max(1 - distance / 15.f, 0);
		color += vec4(c * max(dot(unitToLight, unitNormal), 0.3) * 0.15 * rangecheck, 1);
		specular += c * pow(max(dot(reflect(unitToLight, unitNormal), toCamera), 0), material.shineDump) * ref * rangecheck;
	}
	color += vec4(specular * 0.5, 0);

	color = mix(vec4(0.059, 0.082, 0.106, 1), color, visibility) * c;
}