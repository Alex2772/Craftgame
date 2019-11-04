#pragma once

#include <vector>

#include "gl.h"
#include <glm/glm.hpp>

class SimpleModel {
public:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec4> vertices2;
	std::vector<glm::vec2> texCoords;
	std::vector<GLuint> indices;
	GLuint vao;
	GLuint v = -1;
	GLuint tx = -1;
	GLuint nn = -1;
	GLuint tt = -1;
	GLuint i;
	SimpleModel(std::vector<glm::vec3> vertices, std::vector<glm::vec2> texCoords, std::vector<GLuint> indices);
	SimpleModel(std::vector<glm::vec4> vertices, std::vector<glm::vec2> texCoords, std::vector<GLuint> indices);
	~SimpleModel();
	void flush();
};
