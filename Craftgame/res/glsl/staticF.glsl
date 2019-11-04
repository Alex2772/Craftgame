#version 330

#ifdef SHADOWS
in float layer;
#else
in vec2 pass_texCoords;
in vec3 surfaceNormal;
in mat3 TBN;
in vec3 pass_Position;


layout(location = 0) out vec4 color;
layout(location = 1) out vec4 no;
layout(location = 2) out vec4 f0;

uniform int texturing;
uniform int layer;
uniform vec4 c;
uniform vec3 viewPos;
uniform int lightingC;
struct Material {
	sampler2D tex;
	sampler2D normal;
	vec4 color;
	float roughness;
	vec3 f0;
	int data;
	float refract;
};

uniform Material material;
uniform Material material2;

#ifdef CGE_CUSTOM
vec3 custom_code(vec3 normal);
#endif

vec4 processMaterial(Material material) {
	vec3 normal = surfaceNormal;
	if (bool(material.data & 0x04)) {
		normal = texture(material.normal, pass_texCoords).rgb;
		normal = normalize(normal * 2 - 1);
		normal = normalize(TBN * normal);
	}
	vec3 nN = normalize(normal);
	no = vec4((nN + 1) / 2, material.refract);

#ifdef CGE_CUSTOM
	vec4 rescolor = vec4(custom_code(nN), 1);
#else
	vec4 rescolor;
	if (bool(material.data & 0x01)) {
		rescolor = texture(material.tex, pass_texCoords) * material.color;
	}
	else {
		rescolor = material.color;
	}
#endif
	f0 = vec4(material.f0, material.roughness);
	if (rescolor.a == 0)
		return vec4(0, 0, 0, 0);
	rescolor *= vec4(0.3, 0.3, 0.3, 1);
	
	if (lightingC == 1) {
		float shade = dot(normalize(vec3(-1, 2, 0.8)), no.xyz) * 2.5f + 0.01f;
		rescolor.xyz *= shade;
	}
	return rescolor;
}
#endif

void main(void) {
#ifdef SHADOWS
	gl_FragDepth = gl_FragCoord.z;
	if (layer == 1.f) {
		gl_FragDepth += 0.0006f;
	}
	if (gl_FrontFacing) {
		gl_FragDepth += 0.0003f;
	}
#else
	if (texturing == 1) {
		if (layer >= 0) {
			color = processMaterial(material);
		}
		if (color.a * c.a <= 0) {
			discard;
		}
		if (layer > 0) {
			vec4 s = processMaterial(material2);
			color.xyz = mix(color.xyz, s.xyz, s.a);
		}
			
	}
	else {
		color = vec4(1, 1, 1, 1);
	}
	if (lightingC == 0)
		color *= vec4(c.xyz * 0.5f, c.a);
	else
		color *= c;
#endif
}