#version 330 core


in vec4 position;
in vec2 texCoords;
in vec3 normal;
in vec3 tangent;
#ifndef SHADOWS
out vec3 surfaceNormal;
out vec2 pass_texCoords;
out mat3 TBN;
out vec3 pass_Position;

uniform mat4 projection;
uniform mat4 view;
uniform int block;
#endif
uniform mat4 transform;

void main() {
#ifdef SHADOWS
	gl_Position = transform * position;
#else
	vec4 _worldPos = transform * position;
	gl_Position = projection * view * _worldPos;

	pass_texCoords = texCoords;
	vec3 T;
	vec3 B;
	vec3 N;
	if (block == 1) {
		surfaceNormal = (transform * vec4(0, 0, 1, 0)).xyz;
		T = normalize((transform * vec4(1, 0, 0, 0)).xyz);
		B = normalize((transform * vec4(0, 1, 0, 0)).xyz);
		N = normalize(surfaceNormal);
	}
	else {
		surfaceNormal = (transform * vec4(normal, 0)).xyz;
		N = normalize(surfaceNormal);
		T = normalize((transform * vec4(tangent, 0)).xyz);
		vec3 bt = cross(T, N);
		B = normalize(vec3(transform * vec4(bt, 0.0)));
	}
	TBN = mat3(T, B, N);
	pass_Position = _worldPos.xyz;
#endif
}