#version 330 core
// Лол, мой первый геометрический шейдер

layout(triangles) in;
layout(triangle_strip, max_vertices = 6) out;

uniform mat4 transforms[2];

out float layer;

void main()
{
	for (int j = 0; j < 2; ++j) {
		gl_Layer = j;
		layer = j;
		for (int i = 0; i < 3; ++i) {
			gl_Position = transforms[j] * gl_in[i].gl_Position;
			EmitVertex();
		}
		EndPrimitive();
	}
}