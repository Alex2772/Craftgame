#pragma once

#include <vector>
#include <GL/glew.h>
#include <GL\GL.h>
#include <glm/glm.hpp>

class SimpleModel {
public:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<GLuint> indices;
	GLuint vao;
	GLuint v = -1;
	GLuint tx = -1;
	GLuint i;
	SimpleModel(std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& texCoords, std::vector<GLuint>& indices);
	~SimpleModel();
	void flush();
};