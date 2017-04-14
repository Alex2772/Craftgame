#version 330

in vec3 position;
in vec2 texCoords;
out vec3 pass_Pos;
out vec2 pass_texCoords;

uniform mat4 transform;
uniform mat4 transform2;

void main() {
	gl_Position = transform * vec4(position, 1.0);
	vec4 azaza = transform2 * vec4(position, 1.0);
	pass_Pos = azaza.xyz / azaza.w;
	pass_texCoords = texCoords;
}