#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "gl.h"

class CFace
{
public:
	CFace() {};

	std::vector<glm::vec3> position;
	std::vector<glm::vec2> uv;
	void addVertex(glm::vec3 pos, glm::vec2 uv);
};


class ChunkMesher
{
public:
	ChunkMesher();
	std::vector<glm::vec3> position;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> normal;
	std::vector<glm::vec3> tangent;
	std::vector<GLuint> indices;

	void addFace(CFace& f);
};