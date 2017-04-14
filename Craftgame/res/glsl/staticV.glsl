#version 330

#define LIGHT 16
struct LightSource {
	vec3 pos;
	vec3 color;
};

in vec3 position;
in vec2 texCoords;
in vec3 normal;
in vec3 tangent;
out vec3 surfaceNormal;
out vec2 pass_texCoords;
out vec3 tl[LIGHT];
out vec3 toCamera;
out float visibility;

uniform mat4 transform;
uniform mat4 projection;
uniform mat4 view;
uniform LightSource light[LIGHT];
uniform int useNormal;

#define DENSITY 0.007
#define GRADIENT 2.5

void main() {
	vec4 worldPos = transform * vec4(position, 1.0);
	mat4 pv = projection * view;
	mat4  modelView = view * transform;
	gl_Position = pv * worldPos;

	vec4 relToCam = modelView * vec4(position, 1);
	pass_texCoords = texCoords;
	if (useNormal == 1) {
		surfaceNormal = (modelView * vec4(normal, 0.0)).xyz;

		vec3 normal_o = normalize(surfaceNormal);
		vec3 tangent_o = normalize((modelView * vec4(tangent, 0.0)).xyz);
		vec3 bitangent_o = -normalize((modelView * vec4(cross(surfaceNormal, tangent), 0.0)).xyz);
		mat3 TBN = mat3(
			tangent_o.x, bitangent_o.x, normal_o.x,
			tangent_o.y, bitangent_o.y, normal_o.y,
			tangent_o.z, bitangent_o.z, normal_o.z
		);
		toCamera = TBN * (-relToCam.xyz);
		for (int i = 0; i < LIGHT; i++)
			tl[i] = TBN * ((view * vec4(light[i].pos, 1)).xyz - relToCam.xyz);
	} else {
		surfaceNormal = (transform * vec4(normal, 0)).xyz;
		for (int i = 0; i < LIGHT; i++)
			tl[i] = light[i].pos - worldPos.xyz;
	}
	toCamera = normalize(relToCam.xyz);
	float distance = length(relToCam.xyz);
	visibility = exp(-pow(DENSITY * distance, GRADIENT));
	visibility = clamp(visibility, 0, 1);
}