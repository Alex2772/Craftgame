#version 330

in vec3 position;
in vec2 texCoords;
uniform mat4 transform;


void main() {
	gl_Position = transform * vec4(position, 1.0);
}