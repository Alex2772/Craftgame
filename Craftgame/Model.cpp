#ifndef SERVER
#include "Model.h"

#include "GameEngine.h"

SimpleModel::SimpleModel(std::vector<glm::vec3> _vertices, std::vector<glm::vec2> _texCoords, std::vector<GLuint> _indices) {
	vertices = _vertices;
	texCoords = _texCoords;
	indices = _indices;
	flush();
}

SimpleModel::SimpleModel(std::vector<glm::vec4> _vertices, std::vector<glm::vec2> _texCoords, std::vector<GLuint> _indices)
{
	vertices2 = _vertices;
	texCoords = _texCoords;
	indices = _indices;
	flush();
}

SimpleModel::~SimpleModel() {
	if (vao >= 0) {
		glDeleteBuffers(1, &v);
		glDeleteBuffers(1, &tx);
		glDeleteBuffers(1, &nn);
		glDeleteBuffers(1, &tt);
		glDeleteBuffers(1, &i);
		glDeleteVertexArrays(1, &vao);
	}
}
void SimpleModel::flush() {
	if (vao >= 0) {
		vao = CGE::instance->vao->createVAO();
	}
	if (vertices.empty())
		v = CGE::instance->vao->storeData(0, 4, &vertices2);
	else
		v = CGE::instance->vao->storeData(0, 3, &vertices);
	tx = CGE::instance->vao->storeData(1, 2, &texCoords);
	std::vector<glm::vec3> ss;
	for (char i = 0; i < 6; i++)
		ss.push_back(glm::vec3(0, 1, 0));
	nn = CGE::instance->vao->storeData(2, 3, &ss);
	ss.clear();
	for (char i = 0; i < 6; i++)
		ss.push_back(glm::vec3(0, 0, 1));
	tt = CGE::instance->vao->storeData(3, 3, &ss);
	i = CGE::instance->vao->storeData(5, 1, &indices);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
}
#endif
