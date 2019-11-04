#version 330 core

#define BONES 100
#define BONES_VER_VERTEX 4

struct LightSource {
	vec3 pos;
	vec3 color;
	float distance;
};

in vec4 position;
in vec2 texCoords;
in vec3 normal;
in vec3 tangent;
in int boneIDs1[BONES_VER_VERTEX];
in float boneWeights1[BONES_VER_VERTEX];
in int boneIDs2[BONES_VER_VERTEX];
in float boneWeights2[BONES_VER_VERTEX];

#ifndef SHADOWS
out vec3 surfaceNormal;
out vec2 pass_texCoords;
out vec3 worldPos;
out mat3 TBN;
out vec3 pass_Position;

uniform mat4 projection;
uniform mat4 view;
#endif
uniform mat4 boneMatrix[BONES];
uniform mat4 transform;

void main() {
	mat4 bone = mat4(0.0);
	for (int i = 0; i < BONES_VER_VERTEX; i++) {
		bone += boneMatrix[boneIDs1[i]] * boneWeights1[i];// +boneMatrix[boneIDs2[i]] * boneWeights2[i];
	}
#ifdef SHADOWS
	gl_Position = transform * bone * position;
#else
	vec4 _worldPos = transform * bone * position;
	gl_Position = projection * view * _worldPos;
	worldPos = _worldPos.xyz;
	pass_texCoords = texCoords;
	surfaceNormal = normalize(transform * bone * vec4(normal, 0)).xyz;
	vec3 T = normalize(vec3(transform * bone * vec4(tangent, 0.0)));
	vec3 bt = cross(T, surfaceNormal);

	vec3 B = normalize(vec3(transform * vec4(bt, 0.0)));
	TBN = mat3(T, B, surfaceNormal);
	//float distance = length(relToCam.xyz);
	//visibility = exp(-pow(DENSITY * distance, GRADIENT));
	//visibility = clamp(visibility, 0, 1);
	pass_Position = _worldPos.xyz;
#endif
}