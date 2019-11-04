#version 330

in vec3 position;
in vec2 texCoords;
in vec4 color;
out vec3 pass_Pos;
out vec2 pass_texCoords;
out vec4 pass_Color;

uniform mat4 transform;


void main() {
	gl_Position = transform * vec4(position, 1.0);
	pass_Pos = gl_Position.xyz / gl_Position.w;
	pass_texCoords = texCoords;
	pass_Color = color;
}