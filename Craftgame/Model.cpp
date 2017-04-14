#ifndef SERVER
#include "Model.h"

#include "GameEngine.h"

SimpleModel::SimpleModel(std::vector<glm::vec3>& _vertices, std::vector<glm::vec2>& _texCoords, std::vector<GLuint>& _indices) {
	vertices = _vertices;
	texCoords = _texCoords;
	indices = _indices;
	flush();
}

SimpleModel::~SimpleModel() {
	if (vao >= 0) {
		glDeleteBuffers(1, &v);
		glDeleteBuffers(1, &tx);
		glDeleteBuffers(1, &i);
		glDeleteVertexArrays(1, &vao);
	}
}
void SimpleModel::flush() {
	if (vao >= 0) {
		vao = CGE::instance->vao->createVAO();
	}
	v = CGE::instance->vao->storeData(0, 3, &vertices);
	tx = CGE::instance->vao->storeData(1, 2, &texCoords);
	i = CGE::instance->vao->storeData(2, 1, &indices);
}
#endif